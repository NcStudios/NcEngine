#pragma once

#include "Ecs.h"
#include "framework/Callbacks.h"
#include "ui/dialog/Dialog.h"
#include "ui/dialog/NewSceneDialog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace nc::editor
{
    class AssetManifest;
    class EditorFramework;

    struct ProjectData
    {
        std::string name;
        std::filesystem::path projectDirectory;
        std::vector<std::string> scenes;
        bool open = false;
    };

    /** Manages project and scene creation/loading/etc. */
    class ProjectManager
    {
        public:
            ProjectManager(registry_type* registry, AssetManifest* manifest);

            void OpenProject();
            void CreateProject();
            bool IsProjectOpen() const { return m_projectData.open; }
            auto GetProjectDirectory() const -> const std::filesystem::path& { return m_projectData.projectDirectory; }

            void NewScene();
            void SaveCurrentScene();
            void LoadScene(const std::string& name);
            void ReadNextScene();
            void DeleteCurrentScene();

            void RegisterCallbacks(UICallbacks uiCallbacks, DialogCallbacks dialogCallbacks);

        private:
            registry_type* m_registry;
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