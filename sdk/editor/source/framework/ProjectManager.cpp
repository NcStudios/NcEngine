#include "ProjectManager.h"
#include "ProjectCreation.h"
#include "assets/AssetDependencyChecker.h"
#include "config/ConfigReader.h"
#include "utility/Output.h"
#include "serialize/LoaderScene.h"
#include "serialize/SceneDeleter.h"
#include "serialize/SceneReader.h"
#include "serialize/SceneWriter.h"
#include "EditorScene.h"

#include "graphics/GraphicsModule.h"

#include <cctype>
#include <fstream>

#include <iostream>

namespace
{
    auto ReadScenes(const std::filesystem::path& scenesPath) -> std::vector<std::string>
    {
        std::vector<std::string> out;

        for(const auto& entry : std::filesystem::directory_iterator{scenesPath})
        {
            if(!entry.is_regular_file())
                continue;

            if(entry.path().extension() == nc::editor::FileExtension::Generated)
                out.push_back(entry.path().stem().string());
        }

        return out;
    }
}

namespace nc::editor
{
    ProjectManager::ProjectManager(NcEngine* engine, AssetManifest* manifest)
        : m_engine{engine},
          m_manifest{manifest},
          m_projectData{},
          m_sceneData{},
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
        m_openFileBrowser(std::bind(&ProjectManager::DoOpenProject, this, std::placeholders::_1));
    }

    bool ProjectManager::DoOpenProject(const std::filesystem::path& path)
    {
        auto projectEntry = std::filesystem::directory_entry{path};

        if(!projectEntry.exists() || !projectEntry.is_regular_file())
        {
            Output::LogError("Failure opening project - Invalid path:", path.string());
            return false;
        }

        if(path.extension() != std::filesystem::path{".ncproj"})
        {
            Output::LogError("Failure opening project - Invalid project extension:", path.string());
            return false;
        }

        /** @todo read project file */

        auto projectDirectoryPath = path.parent_path();

        auto sceneDirectoryEntry = std::filesystem::directory_entry{projectDirectoryPath / "scenes"};
        if(!sceneDirectoryEntry.exists())
        {
            Output::Log("Project doesn't have \"scenes\" directory. One will be created.");
            try
            {
                std::filesystem::create_directory(sceneDirectoryEntry.path());
            }
            catch(const std::filesystem::filesystem_error& e)
            {
                Output::LogError("Failed to create \"scenes\" directory:", e.what());
                return false;
            }
        }

        m_projectData.open = true;
        m_projectData.name = path.stem().string();
        m_projectData.projectDirectory = projectDirectoryPath;
        m_projectData.projectFile = path;
        m_projectData.scenes.clear();

        m_projectData.scenes = ReadScenes(sceneDirectoryEntry.path());
        m_uiCallbacks.updateScenes(m_projectData.scenes, 0);
        m_uiCallbacks.setProjectName(m_projectData.name);

        if(!m_projectData.scenes.empty())
            LoadScene(m_projectData.scenes.at(0));

        return true;
    }

    void ProjectManager::CreateProject()
    {
        m_openNewProjectDialog(std::bind(&ProjectManager::DoCreateProject, this, std::placeholders::_1, std::placeholders::_2));
    }

    bool ProjectManager::DoCreateProject(const std::string& name, const std::filesystem::path& path)
    {
        /** @todo save scene/project, if open */

        if(!IsValidProjectParentDirectory(path))
        {
            Output::LogError("Failure creating project - Invalid directory:", path.string());
            return false;
        }

        auto directoryPath = path / name;

        if(std::filesystem::directory_entry{directoryPath}.exists())
        {
            Output::Log("Failure creating project - Project with this name already exists in this directory:", path.string());
            return false;
        }

        Output::Log("Creating Project:", directoryPath.string());

        try
        {
            CreateProjectDirectories(directoryPath);
            CreateProjectFile(directoryPath, name);
            CreateConfig(directoryPath, name);
            CreateMain(directoryPath);
            CopyDefaultAssets(directoryPath);
            CreateCMakeFiles(directoryPath);
        }
        catch(const std::exception& e)
        {
            Output::LogError("Failure creating project:", e.what());
            return false;
        }

        /** @todo default scene? */

        auto projectFilePath = directoryPath / (name + std::string{".ncproj"});
        DoOpenProject(projectFilePath);
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

        m_openNewSceneDialog(std::bind(&ProjectManager::DoNewScene, this, std::placeholders::_1));
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

        Output::Log("Creating scene: " + name);

        SceneWriter writer{m_engine->GetRegistry(), m_manifest, m_projectData.projectDirectory / "scenes"};
        writer.WriteNewScene(name);

        m_projectData.scenes.push_back(name);
        m_uiCallbacks.updateScenes(m_projectData.scenes, static_cast<int>(m_projectData.scenes.size()) - 1);

        LoadScene(name);
        return true;
    }

    void ProjectManager::SaveCurrentScene()
    {
        if(!m_projectData.open)
        {
            Output::LogError("Failure saving scene: No project is open");
            return;
        }

        if(m_projectData.scenes.empty())
        {
            Output::Log("Failure saving scene: No scene is open");
            return;
        }

        if(AssetDependencyChecker checkDependencies{m_engine->GetRegistry(), m_manifest}; !checkDependencies.result)
        {
            Output::Log("Failure saving scene: Missing asset dependencies");
            checkDependencies.LogMissingDependencies();
            return;
        }

        /** @todo This is more of an "on project save" thing, which doesn't exist yet. */
        WriteLoaderScene(m_projectData.projectDirectory, *m_manifest);

        SceneWriter writer{m_engine->GetRegistry(), m_manifest, m_projectData.projectDirectory / "scenes"};
        writer.WriteCurrentScene(&m_sceneData, m_projectData.scenes.at(m_currentSceneIndex));
        Output::Log("Saved scene: " + m_projectData.scenes.at(m_currentSceneIndex));
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
                m_engine->QueueSceneChange(std::make_unique<EditorScene>(this));
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

        auto* registry = m_engine->GetRegistry();
        auto* graphics = m_engine->GetModuleRegistry()->Get<GraphicsModule>();
        NC_TRACE(SceneReader serialize{registry, graphics, m_projectData.projectDirectory / "scenes", m_projectData.scenes.at(m_nextSceneIndex)};);
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
        m_uiCallbacks.updateScenes(m_projectData.scenes, static_cast<int>(m_currentSceneIndex));

        if(!m_projectData.scenes.empty())
            LoadScene(m_projectData.scenes.at(m_currentSceneIndex));
    }
}