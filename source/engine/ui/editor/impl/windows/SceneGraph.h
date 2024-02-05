#pragma once

#include "dialogs/CreateEntityDialog.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

#include "imgui/imgui.h"

#include <string_view>

namespace nc
{
class Tag;
class Transform;

namespace ui::editor
{
class SceneGraph
{
    public:
        void Draw(ecs::Ecs world, CreateEntityDialog& createEntityDialog);
        auto GetSelectedEntity() noexcept -> Entity& { return m_selectedEntity; }

    private:
        Entity m_selectedEntity = Entity::Null();
        ImGuiTextFilter m_tagFilter;

        void EnsureSelection(ecs::Ecs world);
        auto PassFilter(Tag& tag) -> bool;
        void Graph(ecs::Ecs world, CreateEntityDialog& createEntityDialog);
        void GraphNode(ecs::Ecs world, Entity entity, Tag& tag, Transform& transform, CreateEntityDialog& createEntityDialog);
        void GraphContextMenu(ecs::Ecs world, CreateEntityDialog& createEntityDialog);
};
} // namespace ui::editor
} // namespace nc
