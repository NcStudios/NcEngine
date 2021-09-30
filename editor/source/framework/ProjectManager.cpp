#include "ProjectManager.h"
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
    ProjectManager::ProjectManager(registry_type* registry)
        : m_registry{registry},
          m_projectData{},
          m_currentSceneIndex{0u},
          m_nextSceneIndex{0u},
          m_uiCallbacks{},
          m_openFileBrowser{},
          m_openNewSceneDialog{}
    {
    }

    void ProjectManager::OpenProject()
    {
        Output::Log("ProjectManager::OpenProject");

        m_openFileBrowser(std::bind(ProjectManager::DoOpenProject, this, std::placeholders::_1));
    }

    bool ProjectManager::DoOpenProject(const std::filesystem::path& path)
    {
        Output::Log("ProjectManager::DoOpenProject");

        auto projectDirectory = std::filesystem::directory_entry{path};

        if(!projectDirectory.is_directory())
        {
            Output::Log("ProjectManager::OpenProject - Invalid path: " + path.string());
            return false;
        }

        auto sceneDirectory = std::filesystem::directory_entry{path / "scenes"};

        if(!sceneDirectory.exists())
        {
            Output::Log("ProjectManager::OpenProject - Directory does not have \"scenes\" directory");
            return false;
        }

        auto configEntry = std::filesystem::directory_entry{path / "config.ini"};

        if(!configEntry.exists())
        {
            Output::Log("ProjectManager::OpenProject - Could not find config.ini");
            return false;
        }

        //read config
        
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
        Output::Log("ProjectManager::CreateProject");

        m_openFileBrowser(std::bind(ProjectManager::DoCreateProject, this, std::placeholders::_1));
    }

    bool ProjectManager::DoCreateProject(const std::filesystem::path& path)
    {
        Output::Log("ProjectManager::DoCreateProject");

        /** @todo need to specify name */
        std::string name = "New Project";

        {
            auto pathEntry = std::filesystem::directory_entry{path};

            if(!pathEntry.exists())
            {
                Output::Log("ProjectManager::CreateProject - Invalid path");
                Output::Log(path.string());
                return false;
            }

            if(!pathEntry.is_directory())
            {
                Output::Log("ProjectManager::CreatProject - Path is not a directory");
                Output::Log(path.string());
                return false;
            }
        }

        auto directoryPath = path / name;

        {
            std::filesystem::directory_entry directoryEntry{directoryPath};
            if(directoryEntry.exists())
            {
                Output::Log("ProjectManager::CreateProject - Project with this name already exists in this directory");
                Output::Log(path.string());
                return false;
            }
        }

        Output::Log("Creating Project:");
        Output::Log(directoryPath.string());

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
    }

    void ProjectManager::NewScene()
    {
        Output::Log("ProjectManager::NewScene");

        if(m_projectData.name.empty())
        {
            Output::Log("ProjectManager::NewScene - No project is open");
            return;
        }

        m_openNewSceneDialog(std::bind(ProjectManager::DoNewScene, this, std::placeholders::_1));
    }

    bool ProjectManager::DoNewScene(const std::string& name)
    {
        Output::Log("ProjectManager::DoNewScene");

        if(name.empty())
        {
            Output::Log("ProjectManager::DoNewScene - Scene name cannot be empty");
            return false;
        }

        if(!std::isalpha(name[0]))
        {
            Output::Log("ProjectManager::DoNewScene - Scene name must start with a letter");
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
        Output::Log("ProjectManager::SaveCurrentScene");

        if(m_projectData.name.empty())
        {
            Output::Log("ProjectManager::SaveCurrentScene - No project is open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            Output::Log("ProjectManager::SaveCurrentScene - No scene is open");
            return;
        }

        SceneWriter writer{m_registry, m_projectData.projectDirectory / "scenes"};
        writer.WriteCurrentScene(m_projectData.scenes.at(m_currentSceneIndex));
    }

    void ProjectManager::LoadScene(const std::string& name)
    {
        Output::Log("ProjectManager::LoadScene");

        if(m_projectData.name.empty())
        {
            Output::Log("ProjectManager:LoadScene - No project open");
            return;
        }

        for(size_t i = 0u; i < m_projectData.scenes.size(); ++i)
        {
            if(m_projectData.scenes.at(i) == name)
            {
                m_nextSceneIndex = i;
                scene::Change(std::make_unique<EditorScene>(this));
                return;
            }
        }

        Output::Log("ProjectManager::LoadScene - Could not find scene: " + name);
    }

    void ProjectManager::ReadNextScene()
    {
        Output::Log("ProjectManager::ReadNextScene");

        if(m_projectData.name.empty())
        {
            Output::Log("ProjectManager:LoadNextScene - No project open");
            return;
        }

        m_currentSceneIndex = m_nextSceneIndex;

        SceneReader serialize{m_registry, m_projectData.projectDirectory / "scenes", m_projectData.scenes.at(m_nextSceneIndex)};
    }

    void ProjectManager::DeleteCurrentScene()
    {
        if(m_projectData.name.empty())
        {
            Output::Log("ProjectManager::DeleteCurrentScene - No project is open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            Output::Log("ProjectManager::DeleteCurrentScene - No scene is open");
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