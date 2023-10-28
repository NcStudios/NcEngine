#pragma once

#include "ncengine/ecs/Entity.h"

#include "imgui/imgui.h"

#include <string_view>

namespace nc
{
class Registry;
class Tag;
class Transform;

namespace ui::editor
{
class SceneGraph
{
    public:
        void Draw(Registry* registry);

        auto GetSelectedEntity() const noexcept -> Entity
        {
            return m_selectedEntity;
        }

    private:
        Entity m_selectedEntity = Entity::Null();
        ImGuiTextFilter m_tagFilter;

        void EnsureSelection(Registry* registry);
        auto PassFilter(Tag& tag) -> bool;
        void Graph(Registry* registry);
        void GraphNode(Registry* registry, Entity entity, Tag& tag, Transform& transform);
        void GraphContextMenu(Registry* registry);
};
} // namespace ui::editor
} // namespace nc
