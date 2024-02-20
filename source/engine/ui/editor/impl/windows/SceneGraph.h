#pragma once

#include "ui/editor/Editor.h"
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
        explicit SceneGraph(EditorContext& ctx);
        void Draw(EditorContext& ctx, CreateEntityDialog& createEntity);
        void OnClose(EditorContext& ctx);

    private:
        Entity m_selectedEntityWireframe = Entity::Null();
        Entity m_selectedColliderWireframe = Entity::Null();
        ImGuiTextFilter m_tagFilter;

        void SetEntitySelection(EditorContext& ctx, Entity entity);
        void EnsureSelection(EditorContext& ctx);
        auto PassFilter(Tag& tag) -> bool;
        void Graph(EditorContext& ctx, CreateEntityDialog& createEntity);
        void GraphNode(EditorContext& ctx, Entity entity, Tag& tag, Hierarchy& hierarchy, CreateEntityDialog& createEntity);
        void GraphContextMenu(EditorContext& ctx, CreateEntityDialog& createEntity);
};
} // namespace ui::editor
} // namespace nc
