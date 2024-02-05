#pragma once

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
        void Draw(ecs::Ecs world);
        auto GetSelectedEntity() const noexcept -> Entity { return m_selectedEntity; }
        void OnClose(ecs::Ecs world);

    private:
        Entity m_selectedEntity = Entity::Null();
        Entity m_selectedEntityWireframe = Entity::Null();
        Entity m_selectedColliderWireframe = Entity::Null();
        ImGuiTextFilter m_tagFilter;

        void SetEntitySelection(ecs::Ecs world, Entity entity);
        void EnsureSelection(ecs::Ecs world);
        auto PassFilter(Tag& tag) -> bool;
        void Graph(ecs::Ecs world);
        void GraphNode(ecs::Ecs world, Entity entity, Tag& tag, Transform& transform);
        void GraphContextMenu(ecs::Ecs world);
};
} // namespace ui::editor
} // namespace nc
