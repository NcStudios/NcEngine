#include "EditorFramework.h"
#include "Callbacks.h"
#include "ui/UIStyle.h"
#include "utility/DefaultComponents.h"

#include "ui/UISystem.h"

namespace
{
    auto CreateProjectCallbacks(nc::editor::ProjectManager* projectManager) -> nc::editor::ProjectCallbacks
    {
        return nc::editor::ProjectCallbacks
        {
            .createProject = [projectManager]{ return projectManager->CreateProject(); },
            .openProject = [projectManager]{ return projectManager->OpenProject(); },
            .getProjectData = [projectManager] () -> const nc::editor::ProjectData& { return projectManager->GetProjectData(); }
        };
    }

    auto CreateSceneCallbacks(nc::editor::ProjectManager* projectManager) -> nc::editor::SceneCallbacks
    {
        return nc::editor::SceneCallbacks
        {
            .newScene = [projectManager]{ return projectManager->NewScene(); },
            .saveScene = [projectManager]{ return projectManager->SaveCurrentScene(); },
            .changeScene = [projectManager](const std::string& name){ return projectManager->LoadScene(name); },
            .deleteCurrentScene = [projectManager]{ return projectManager->DeleteCurrentScene(); }
        };
    }

    auto CreateUICallbacks(nc::editor::EditorUI* ui) -> nc::editor::UICallbacks
    {
        return nc::editor::UICallbacks
        {
            .registerDialog = ui->GetRegisterDialogCallback(),
            .updateScenes = [ui](std::vector<std::string> scenes, int selectedScene){ return ui->UpdateScenes(std::move(scenes), selectedScene); },
            .setProjectName = [ui](std::string name){ return ui->SetProjectName(std::move(name)); }
        };
    }

    auto CreateDialogCallbacks(nc::editor::FileBrowser* fileBrowser,
                               nc::editor::NewSceneDialog* newSceneDialog,
                               nc::editor::NewProjectDialog* newProjectDialog,
                               nc::editor::AssetBrowser* assetBrowser,
                               nc::editor::ConfigEditor* configEditor) -> nc::editor::DialogCallbacks
    {
        using FileBrowserCallback = nc::editor::DialogCallbacks::FileBrowserOnConfirmCallbackType;
        using newScene = nc::editor::DialogCallbacks::NewSceneOnConfirmCallbackType;
        using OpenProjectCallback = nc::editor::DialogCallbacks::NewProjectOnConfirmCallbackType;

        return nc::editor::DialogCallbacks
        {
            .openFileBrowser = [fileBrowser](FileBrowserCallback callback){ fileBrowser->Open(std::move(callback)); },
            .openNewSceneDialog = [newSceneDialog](newScene callback){ newSceneDialog->Open(std::move(callback)); },
            .openNewProjectDialog = [newProjectDialog](OpenProjectCallback callback){ newProjectDialog->Open(std::move(callback)); },
            .openAssetBrowser = [assetBrowser](){ return assetBrowser->Open(); },
            .openConfigEditor = [configEditor](const std::filesystem::path& path){ configEditor->Open(path); }
        };
    }

    auto CreateOpenFileBrowserCallback(nc::editor::FileBrowser* fileBrowser) -> nc::editor::DialogCallbacks::OpenFileBrowserCallbackType
    {
        return [fileBrowser](nc::editor::DialogCallbacks::FileBrowserOnConfirmCallbackType callback){ fileBrowser->Open(std::move(callback)); };
    }
}

namespace nc::editor
{
    EditorFramework::EditorFramework(NcEngine* engine)
        : m_output{},
          m_editorConfig{ReadConfig("editor/config/editor_config.ini")},
          m_assetManifest{m_editorConfig.recentProjectDirectory, [configEditor = &m_configEditor]() -> const config::Config& { return configEditor->GetConfig();}},
          m_projectManager{engine, &m_assetManifest},
          m_environment2{m_projectManager.GetSceneData(), engine->Registry(), &m_assetManifest, engine->Environment()},
          m_editorUI{engine->Registry(),
                     &m_output,
                     &m_environment2,
                     &m_assetManifest,
                     CreateProjectCallbacks(&m_projectManager),
                     CreateSceneCallbacks(&m_projectManager),
                     std::bind(ChangeTagDialog::Open, &m_changeTagDialog, std::placeholders::_1),
                     std::string{}},
          m_fileBrowser{m_editorUI.GetRegisterDialogCallback()},
          m_assetBrowser{m_editorUI.GetRegisterDialogCallback(), CreateOpenFileBrowserCallback(&m_fileBrowser), &m_assetManifest},
          m_newSceneDialog{m_editorUI.GetRegisterDialogCallback()},
          m_newProjectDialog{m_editorUI.GetRegisterDialogCallback()},
          m_changeTagDialog{engine->Registry()},
          m_configEditor{m_editorConfig.recentProjectDirectory / "config\\config.ini"}
    {
        nc::editor::SetImGuiStyle();
        engine->UI()->Set(&m_editorUI);

        auto uiCallbacks = CreateUICallbacks(&m_editorUI);
        m_changeTagDialog.RegisterDialog(uiCallbacks.registerDialog);
        m_configEditor.RegisterDialog(uiCallbacks.registerDialog);

        auto dialogCallbacks = CreateDialogCallbacks(&m_fileBrowser, &m_newSceneDialog, &m_newProjectDialog, &m_assetBrowser, &m_configEditor);
        m_editorUI.RegisterCallbacks(dialogCallbacks.openAssetBrowser, dialogCallbacks.openConfigEditor);

        m_projectManager.RegisterCallbacks(std::move(uiCallbacks), std::move(dialogCallbacks));

        m_projectManager.DoOpenProject(m_editorConfig.recentProjectFilePath);
    }

    void EditorFramework::SaveProjectData()
    {
        m_editorConfig.recentProjectDirectory = m_projectManager.GetProjectDirectory();
        m_editorConfig.recentProjectFilePath = m_projectManager.GetProjectFilePath();
        WriteConfig("editor/config/editor_config.ini", m_editorConfig);

        if(m_projectManager.IsProjectOpen())
            m_assetManifest.Write(m_projectManager.GetProjectDirectory());
    }
}