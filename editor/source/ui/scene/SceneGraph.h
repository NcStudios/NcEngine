#pragma once

#include "ecs/Registry.h"
#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"
#include "ui/dialog/Dialog.h"
#include "Inspector.h"
#include "SceneManagementControl.h"

#include <functional>

namespace nc::editor
{
    class AssetManifest;

    class SceneGraph : public window::IOnResizeReceiver
    {
        public:
            SceneGraph(Registry* registry,
                       AssetManifest* assetManifest,
                       SceneCallbacks sceneCallbacks,
                       EntityCallbacks::ChangeTagCallbackType changeTag,
                       std::string projectName);
            ~SceneGraph();
            void Draw();
            void OnResize(Vector2 dimensions) override;

            void SetProjectName(std::string name);
            void UpdateScenes(std::vector<std::string> scenes, int selectedScene);

        private:
            Registry* m_registry;
            AssetManifest* m_assetManifest;
            std::string m_projectName;
            Inspector m_inspector;
            SceneManagementControl m_sceneManagementControl;
            EntityCallbacks::ChangeTagCallbackType m_changeTag;
            Vector2 m_dimensions;
            Entity m_selectedEntity;

            void SceneGraphPanel();
            void SceneGraphNode(Entity entity, Tag* tag, Transform* transform);
            void EntityPanel(Entity entity);
            void StateAttachmentElement(StateAttachment* comp);
            void SceneGraphContextMenu();
            void EntityContextMenu(Entity entity);
    };
}