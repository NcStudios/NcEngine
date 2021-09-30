#pragma once

#include "Ecs.h"
#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"
#include "ui/dialog/Dialog.h"
#include "Inspector.h"
#include "SceneSelectWidget.h"

#include <functional>

namespace nc::editor
{
    class SceneGraph : public window::IOnResizeReceiver
    {
        public:
            SceneGraph(registry_type* registry, SceneCallbacks sceneCallbacks, EntityCallbacks::ChangeTagCallbackType changeTagCallback, std::string projectName);
            ~SceneGraph();
            void Draw();
            void OnResize(Vector2 dimensions) override;

            void SetProjectName(std::string name);
            void UpdateScenes(std::vector<std::string> scenes, int selectedScene);

        private:
            registry_type* m_registry;
            std::string m_projectName;
            Inspector m_inspector;
            SceneSelectWidget m_sceneSelectWidget;
            EntityCallbacks::ChangeTagCallbackType m_changeTagCallback;
            Vector2 m_dimensions;
            EntityTraits::underlying_type m_selectedEntity;

            void SceneGraphPanel();
            void SceneGraphNode(Entity entity, Tag* tag, Transform* transform);
            void EntityPanel(Entity entity);
            void AutoComponentElement(AutoComponent* comp);
            void SceneGraphContextMenu();
            void EntityContextMenu(Entity entity);
    };
}