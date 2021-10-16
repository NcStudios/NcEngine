#include "ProjectManager.h"
#include "assets/AssetDependencyChecker.h"
#include "config/ConfigReader.h"
#include "utility/Output.h"
#include "serialize/SceneDeleter.h"
#include "serialize/SceneReader.h"
#include "serialize/SceneWriter.h"
#include "EditorScene.h"

#include <cctype>
#include <fstream>

#include <iostream>

namespace
{
    void CreateProjectDirectories(const std::filesystem::path& path)
    {
        std::filesystem::create_directories(path);
        std::filesystem::create_directory(path / "assets");
        std::filesystem::create_directory(path / "assets" / "mesh");
        std::filesystem::create_directory(path / "assets" / "mesh_colliders");
        std::filesystem::create_directory(path / "assets" / "sounds");
        std::filesystem::create_directory(path / "models");
        std::filesystem::create_directory(path / "scenes");
        std::filesystem::create_directory(path / "shaders");
        std::filesystem::create_directory(path / "source");
        std::filesystem::create_directory(path / "textures");
    }

    auto ReadScenes(const std::filesystem::path& scenesPath) -> std::vector<std::string>
    {
        std::vector<std::string> out;

        for(const auto& entry : std::filesystem::directory_iterator{scenesPath})
        {
            if(!entry.is_regular_file())
                continue;
            
            if(entry.path().extension() == ".gen")
                out.push_back(entry.path().stem().string());
        }

        return out;
    }
}

namespace nc::editor
{
    ProjectManager::ProjectManager(registry_type* registry, AssetManifest* manifest)
        : m_registry{registry},
          m_manifest{manifest},
          m_projectData{},
          m_currentSceneIndex{0u},
          m_nextSceneIndex{0u},
          m_uiCallbacks{},
          m_openFileBrowser{},
          m_openNewSceneDialog{},
          m_openNewProjectDialog{}
    {
    }

    void ProjectManager::OpenProject()
    {
        m_openFileBrowser(std::bind(ProjectManager::DoOpenProject, this, std::placeholders::_1));
    }

    bool ProjectManager::DoOpenProject(const std::filesystem::path& path)
    {
        auto projectDirectory = std::filesystem::directory_entry{path};

        if(!projectDirectory.is_directory())
        {
            Output::LogError("Failure opening project - Invalid path:", path.string());
            return false;
        }

        auto sceneDirectory = std::filesystem::directory_entry{path / "scenes"};

        if(!sceneDirectory.exists())
        {
            Output::Log("Project doesn't have \"scenes\" directory. One will be created.");
            std::filesystem::create_directory(sceneDirectory.path());
        }

        auto configEntry = std::filesystem::directory_entry{path / "config.ini"};

        if(!configEntry.exists())
        {
            Output::LogError("Failure opening project - Could not find config.ini");
            return false;
        }

        /** @todo read config */
        
        m_projectData.open = true;
        m_projectData.name = "Sample Project";
        m_projectData.projectDirectory = path;
        m_projectData.scenes.clear();

        m_projectData.scenes = ReadScenes(sceneDirectory.path());
        m_uiCallbacks.updateScenesCallback(m_projectData.scenes, 0);
        m_uiCallbacks.setProjectNameCallback(m_projectData.name);

        if(!m_projectData.scenes.empty())
            LoadScene(m_projectData.scenes.at(0));

        return true;
    }

    void ProjectManager::CreateProject()
    {
        m_openNewProjectDialog(std::bind(ProjectManager::DoCreateProject, this, std::placeholders::_1, std::placeholders::_2));
    }

    bool ProjectManager::DoCreateProject(const std::string& name, const std::filesystem::path& path)
    {
        {
            auto pathEntry = std::filesystem::directory_entry{path};

            if(!pathEntry.exists())
            {
                Output::LogError("Failure creating project - Invalid path:", path.string());
                return false;
            }

            if(!pathEntry.is_directory())
            {
                Output::Log("Failure creating project - Path is not a directory:", path.string());
                return false;
            }
        }

        auto directoryPath = path / name;

        {
            std::filesystem::directory_entry directoryEntry{directoryPath};
            if(directoryEntry.exists())
            {
                Output::Log("Failure creating project - Project with this name already exists in this directory:", path.string());
                return false;
            }
        }

        Output::Log("Creating Project:", directoryPath.string());

        CreateProjectDirectories(directoryPath);
        
        std::ofstream projectConfig{directoryPath / "config.ini"};
        projectConfig << "name=" << name << '\n'
                      << "initial_scene=" << "SampleScene";
        projectConfig.close();

        DoOpenProject(directoryPath);

        return true;
    }

    void ProjectManager::RegisterCallbacks(UICallbacks uiCallbacks, DialogCallbacks dialogCallbacks)
    {
        m_uiCallbacks = std::move(uiCallbacks);
        m_openFileBrowser = std::move(dialogCallbacks.openFileBrowser);
        m_openNewSceneDialog = std::move(dialogCallbacks.openNewSceneDialog);
        m_openNewProjectDialog = std::move(dialogCallbacks.openNewProjectDialog);
    }

    void ProjectManager::NewScene()
    {
        if(m_projectData.name.empty())
        {
            Output::LogError("Failure creating scene: No project is open");
            return;
        }

        m_openNewSceneDialog(std::bind(ProjectManager::DoNewScene, this, std::placeholders::_1));
    }

    bool ProjectManager::DoNewScene(const std::string& name)
    {
        if(name.empty())
        {
            Output::LogError("Failure creating scene: name cannot be empty");
            return false;
        }

        /** @todo we need to enforce scene name is a valid c++ identifier or change how that is handled. */
        if(!std::isalpha(name[0]))
        {
            Output::LogError("Failure creating scene: name must start with a letter");
            return false;
        }

        SceneWriter writer{m_registry, m_projectData.projectDirectory / "scenes"};
        writer.WriteNewScene(name);

        m_projectData.scenes.push_back(name);
        m_uiCallbacks.updateScenesCallback(m_projectData.scenes, m_projectData.scenes.size() - 1);

        LoadScene(name);
        return true;
    }

    void ProjectManager::SaveCurrentScene()
    {
        if(m_projectData.name.empty())
        {
            Output::LogError("Failure saving scene: No project is open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            Output::Log("Failure saving scene: No scene is open");
            return;
        }

        if(AssetDependencyChecker checkDependencies{m_registry, m_manifest}; !checkDependencies.result)
        {
            Output::Log("Failure saving scene: Missing asset dependencies");
            checkDependencies.LogMissingDependencies();
            return;
        }

        SceneWriter writer{m_registry, m_projectData.projectDirectory / "scenes"};
        writer.WriteCurrentScene(m_projectData.scenes.at(m_currentSceneIndex));
    }

    void ProjectManager::LoadScene(const std::string& name)
    {
        if(m_projectData.name.empty())
        {
            Output::LogError("Failure loading scene: No project open");
            return;
        }

        for(size_t i = 0u; i < m_projectData.scenes.size(); ++i)
        {
            if(m_projectData.scenes.at(i) == name)
            {
                m_nextSceneIndex = i;
                Output::Log("Loading scene: " + name);
                scene::Change(std::make_unique<EditorScene>(this));
                return;
            }
        }

        Output::LogError("Could not find scene: " + name);
    }

    void ProjectManager::ReadNextScene()
    {
        if(m_projectData.name.empty())
        {
            Output::LogError("ProjectManager:LoadNextScene - No project open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            return;
        }

        m_currentSceneIndex = m_nextSceneIndex;

        NC_TRACE(SceneReader serialize{m_registry, m_projectData.projectDirectory / "scenes", m_projectData.scenes.at(m_nextSceneIndex)};);
    }

    void ProjectManager::DeleteCurrentScene()
    {
        if(m_projectData.name.empty())
        {
            Output::LogError("Failure deleting scene: No project is open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            Output::LogError("Failure deleting scene: No scene is open");
            return;
        }

        DeleteScene(m_projectData.projectDirectory / "scenes", m_projectData.scenes.at(m_currentSceneIndex));

        m_projectData.scenes = ReadScenes(m_projectData.projectDirectory / "scenes");
        m_currentSceneIndex = m_projectData.scenes.empty() ? 0 : m_projectData.scenes.size() - 1;
        m_uiCallbacks.updateScenesCallback(m_projectData.scenes, m_currentSceneIndex);

        if(!m_projectData.scenes.empty())
            LoadScene(m_projectData.scenes.at(m_currentSceneIndex));
    }
}