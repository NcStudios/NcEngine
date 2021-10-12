#include "EditorFramework.h"
#include "Callbacks.h"
#include "ui/UIStyle.h"
#include "utility/DefaultComponents.h"

#include "UI.h"

namespace
{
    auto CreateProjectCallbacks(nc::editor::ProjectManager* projectManager) -> nc::editor::ProjectCallbacks
    {
        return nc::editor::ProjectCallbacks
        {
            .createProjectCallback = std::bind(nc::editor::ProjectManager::CreateProject, projectManager),
            .openProjectCallback = std::bind(nc::editor::ProjectManager::OpenProject, projectManager)
        };
    }

    auto CreateSceneCallbacks(nc::editor::ProjectManager* projectManager) -> nc::editor::SceneCallbacks
    {
        return nc::editor::SceneCallbacks
        {
            .newSceneCallback = std::bind(nc::editor::ProjectManager::NewScene, projectManager),
            .saveSceneCallback = std::bind(nc::editor::ProjectManager::SaveCurrentScene, projectManager),
            .changeSceneCallback = std::bind(nc::editor::ProjectManager::LoadScene, projectManager, std::placeholders::_1),
            .deleteCurrentSceneCallback = std::bind(nc::editor::ProjectManager::DeleteCurrentScene, projectManager)
        };
    }

    auto CreateUICallbacks(nc::editor::EditorUI* ui) -> nc::editor::UICallbacks
    {
        return nc::editor::UICallbacks
        {
            .addDialogCallback = std::bind(nc::editor::EditorUI::AddDialog, ui, std::placeholders::_1),
            .updateScenesCallback = std::bind(nc::editor::EditorUI::UpdateScenes, ui, std::placeholders::_1, std::placeholders::_2),
            .setProjectNameCallback = std::bind(nc::editor::EditorUI::SetProjectName, ui, std::placeholders::_1)
        };
    }

    auto CreateDialogCallbacks(nc::editor::FileBrowser* fileBrowser,
                               nc::editor::NewSceneDialog* newSceneDialog) -> nc::editor::DialogCallbacks
    {
        return nc::editor::DialogCallbacks
        {
            .openFileBrowser = std::bind(nc::editor::FileBrowser::Open, fileBrowser, std::placeholders::_1),
            .openNewSceneDialog = std::bind(nc::editor::NewSceneDialog::Open, newSceneDialog, std::placeholders::_1)
        };
    }
}

namespace nc::editor
{
    EditorFramework::EditorFramework(registry_type* registry)
        : m_output{},
          m_editorConfig{ReadConfig("editor/config.ini")},
          m_assetManifest{m_editorConfig.recentProjectDirectory},
          m_projectManager{registry, &m_assetManifest},
          m_editorUI{registry,
                     &m_output,
                     &m_assetManifest,
                     CreateProjectCallbacks(&m_projectManager),
                     CreateSceneCallbacks(&m_projectManager),
                     std::bind(ChangeTagDialog::Open, &m_changeTagDialog, std::placeholders::_1),
                     std::string{}},
          m_fileBrowser{std::bind(EditorUI::AddDialog, &m_editorUI, std::placeholders::_1)},
          m_newSceneDialog{std::bind(EditorUI::AddDialog, &m_editorUI, std::placeholders::_1)},
          m_changeTagDialog{registry}
    {
        nc::editor::SetImGuiStyle();
        nc::ui::Set(&m_editorUI);

        auto uiCallbacks = CreateUICallbacks(&m_editorUI);
        m_changeTagDialog.RegisterAddDialogCallback(uiCallbacks.addDialogCallback);

        auto dialogCallbacks = CreateDialogCallbacks(&m_fileBrowser, &m_newSceneDialog);
        m_editorUI.RegisterCallbacks(dialogCallbacks.openFileBrowser);
        
        m_projectManager.RegisterCallbacks(std::move(uiCallbacks), std::move(dialogCallbacks));

        m_projectManager.DoOpenProject(m_editorConfig.recentProjectDirectory);
    }

    void EditorFramework::SaveProjectData()
    {
        m_editorConfig.recentProjectDirectory = m_projectManager.GetProjectDirectory();
        WriteConfig("editor/config.ini", m_editorConfig);

        if(m_projectManager.IsProjectOpen())
            m_assetManifest.Write(m_projectManager.GetProjectDirectory());
    }
}