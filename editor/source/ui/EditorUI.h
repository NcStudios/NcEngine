#pragma once

#include "ecs/Registry.h"
#include "ui/IUI.h"
#include "window/IOnResizeReceiver.h"
#include "framework/Callbacks.h"
#include "ui/Environment.h"
#include "ui/dialog/Dialog.h"
#include "ui/dialog/ActiveDialogs.h"
#include "scene/SceneGraph.h"
#include "UtilitiesPanel.h"

namespace nc::editor
{
    class Output;
    class ProjectManager;

    /** Core UI registered with the engine. This Draw() is the first called
     *  during rendering. */
    class EditorUI : public ui::UIFlexible, public window::IOnResizeReceiver
    {
        public:
            EditorUI(Registry* registry,
                     Output* output,
                     Environment* environment,
                     AssetManifest* assetManifest,
                     ProjectCallbacks projectCallbacks,
                     SceneCallbacks sceneCallbacks,
                     EntityCallbacks::ChangeTagCallbackType changeTag,
                     std::string projectName);
            ~EditorUI();

            void Draw() override;
            bool IsHovered() override;
            void OnResize(Vector2 dimensions) override;

            void RegisterCallbacks(DialogCallbacks::OpenAssetBrowserCallbackType openAssetBrowser,
                                   DialogCallbacks::OpenConfigEditorCallbackType openConfigEditor);

            void SetProjectName(std::string name);
            void UpdateScenes(std::vector<std::string> scenes, int selectedScene);

            auto GetRegisterDialogCallback() -> UICallbacks::RegisterDialogCallbackType;

        private:
            Registry* m_registry;
            Vector2 m_dimensions;
            ProjectCallbacks m_callbacks;
            SceneGraph m_sceneGraph;
            UtilitiesPanel m_utilitiesPanel;
            ActiveDialogs m_activeDialogs;
            DialogCallbacks::OpenAssetBrowserCallbackType m_openAssetBrowser;
            DialogCallbacks::OpenConfigEditorCallbackType m_openConfigEditor;
            void Menu();
    };
}