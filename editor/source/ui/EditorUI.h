#pragma once

#include "Ecs.h"
#include "ui/IUI.h"
#include "window/IOnResizeReceiver.h"

#include "framework/Callbacks.h"
#include "ui/dialog/Dialog.h"
#include "ui/dialog/AssetBrowser.h"
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
            EditorUI(registry_type* registry,
                     Output* output,
                     AssetManifest* assetManifest,
                     ProjectCallbacks projectCallbacks,
                     SceneCallbacks sceneCallbacks,
                     EntityCallbacks::ChangeTagCallbackType changeTagCallback,
                     std::string projectName);
            ~EditorUI();

            void Draw() override;
            bool IsHovered() override;
            void OnResize(Vector2 dimensions) override;

            void RegisterCallbacks(DialogCallbacks::OpenFileBrowserCallbackType callback);

            void AddDialog(DialogBase* dialog);
            void SetProjectName(std::string name);
            void UpdateScenes(std::vector<std::string> scenes, int selectedScene);

        private:
            registry_type* m_registry;
            Vector2 m_dimensions;
            ProjectCallbacks m_callbacks;
            SceneGraph m_sceneGraph;
            UtilitiesPanel m_utilitiesPanel;
            AssetBrowser m_assetBrowser;
            std::vector<DialogBase*> m_dialogs;
            void Menu();
    };
}