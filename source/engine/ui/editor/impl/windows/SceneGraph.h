#pragma once

#include "ui/editor/Editor.h"
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
        void Draw(EditorContext& ctx);
        void OnClose(EditorContext& ctx);

    private:
        Entity m_selectedEntityWireframe = Entity::Null();
        Entity m_selectedColliderWireframe = Entity::Null();
        ImGuiTextFilter m_tagFilter;

        void SetEntitySelection(EditorContext& ctx, Entity entity);
        void EnsureSelection(EditorContext& ctx);
        auto PassFilter(Tag& tag) -> bool;
        void Graph(EditorContext& ctx);
        void GraphNode(EditorContext& ctx, Entity entity, Tag& tag, Transform& transform);
        void GraphContextMenu(EditorContext& ctx);
};
} // namespace ui::editor
} // namespace nc
