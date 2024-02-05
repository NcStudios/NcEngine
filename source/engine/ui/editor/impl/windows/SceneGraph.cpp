#include "SceneGraph.h"
#include "EntityContextMenu.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
auto IsRoot(nc::Transform& transform) -> bool
{
    return !transform.Parent().Valid();
}
} // anonymous namespace

namespace nc::ui::editor
{
void SceneGraph::Draw(ecs::Ecs world, CreateEntityDialog& createEntityDialog)
{
    EnsureSelection(world);
    m_tagFilter.Draw("search");

    ChildWindow("Entity List", [&]()
    {
        if (IsWindowBackgroundClicked())
        {
            m_selectedEntity = Entity::Null();
        }

        GraphContextMenu(world, createEntityDialog);
        Graph(world, createEntityDialog);
    });
}

void SceneGraph::EnsureSelection(ecs::Ecs world)
{
    if (m_selectedEntity.Valid() && !world.Contains<Entity>(m_selectedEntity))
    {
        m_selectedEntity = Entity::Null();
    }
}

auto SceneGraph::PassFilter(Tag& tag) -> bool
{
    return m_tagFilter.PassFilter(tag.Value().data());
}

void SceneGraph::Graph(ecs::Ecs world, CreateEntityDialog& createEntityDialog)
{
    for(auto entity : world.GetAll<Entity>())
    {
        auto& transform = world.Get<Transform>(entity);
        auto& tag = world.Get<Tag>(entity);
        if (IsRoot(transform) && PassFilter(tag))
        {
            GraphNode(world, entity, tag, transform, createEntityDialog);
        }
    }
}

void SceneGraph::GraphNode(ecs::Ecs world, Entity entity, Tag& tag, Transform& transform, CreateEntityDialog& createEntityDialog)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = m_selectedEntity == entity
        ? ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow
        : 0;

    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
    }
    else if (ImGui::BeginPopupContextItem())
    {
        m_selectedEntity = EntityContextMenu(entity, world, createEntityDialog);
        ImGui::EndPopup();
    }

    DragAndDropSource<Entity>(&entity);
    DragAndDropTarget<Entity>([entity, world](Entity* source) mutable
    {
        world.Get<Transform>(*source).SetParent(entity);
    });

    if(isNodeExpanded)
    {
        for(auto child : transform.Children())
        {
            GraphNode(world, child, world.Get<Tag>(child), world.Get<Transform>(child), createEntityDialog);
        }

        ImGui::TreePop();
    }
}

void SceneGraph::GraphContextMenu(ecs::Ecs world, CreateEntityDialog& createEntityDialog)
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("New Entity"))
            createEntityDialog.Open();
        else if (ImGui::Selectable("New Default Entity"))
            m_selectedEntity = world.Emplace<Entity>(EntityInfo{});
        else
            m_selectedEntity = Entity::Null();

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
