#pragma once

#include "Ecs.h"
#include "EditorConfig.h"
#include "assets/AssetManifest.h"
#include "framework/ProjectManager.h"
#include "ui/EditorUI.h"
#include "ui/dialog/ChangeTagDialog.h"
#include "ui/dialog/FileBrowser.h"
#include "ui/dialog/NewSceneDialog.h"

namespace nc::editor
{
    /** Creates and sets callbacks for all of the core editor classes. */
    class EditorFramework
    {
        public:
            EditorFramework(registry_type* registry);
            EditorFramework(const EditorFramework&) = delete;
            EditorFramework(EditorFramework&&) = delete;
            EditorFramework& operator=(const EditorFramework&) = delete;
            EditorFramework& operator=(EditorFramework&&) = delete;

            void SaveProjectData();
            auto GetProjectManager() -> ProjectManager* { return &m_projectManager; }

        private:
            Output m_output;
            EditorConfig m_editorConfig;
            AssetManifest m_assetManifest;
            ProjectManager m_projectManager;
            EditorUI m_editorUI;
            FileBrowser m_fileBrowser;
            NewSceneDialog m_newSceneDialog;
            ChangeTagDialog m_changeTagDialog;
    };
}