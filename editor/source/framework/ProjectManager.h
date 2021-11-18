#pragma once

#include "NcEngine.h"
#include "Callbacks.h"
#include "ProjectData.h"
#include "ui/dialog/Dialog.h"
#include "ui/dialog/NewSceneDialog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace nc::editor
{
    class AssetManifest;
    class EditorFramework;

    /** Manages project and scene creation/loading/etc. */
    class ProjectManager
    {
        public:
            ProjectManager(NcEngine* engine, AssetManifest* manifest);

            void OpenProject();
            void CreateProject();
            bool IsProjectOpen() const { return m_projectData.open; }
            auto GetProjectData() const -> const ProjectData& { return m_projectData; }
            auto GetProjectDirectory() const -> const std::filesystem::path& { return m_projectData.projectDirectory; }
            auto GetProjectFilePath() const -> const std::filesystem::path& { return m_projectData.projectFile; }

            void NewScene();
            void SaveCurrentScene();
            void LoadScene(const std::string& name);
            void ReadNextScene();
            void DeleteCurrentScene();

            void RegisterCallbacks(UICallbacks uiCallbacks, DialogCallbacks dialogCallbacks);

        private:
            NcEngine* m_engine;
            AssetManifest* m_manifest;
            ProjectData m_projectData;
            size_t m_currentSceneIndex;
            size_t m_nextSceneIndex;
            UICallbacks m_uiCallbacks;
            DialogCallbacks::OpenFileBrowserCallbackType m_openFileBrowser;
            DialogCallbacks::OpenNewSceneDialogCallbackType m_openNewSceneDialog;
            DialogCallbacks::OpenNewProjectDialogCallbackType m_openNewProjectDialog;

            bool DoOpenProject(const std::filesystem::path& path);
            bool DoCreateProject(const std::string& name, const std::filesystem::path& path);
            bool DoNewScene(const std::string& name);
    
            friend EditorFramework;
    };
}