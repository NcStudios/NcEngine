#include "SceneGraph.h"

#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
auto IsRoot(nc::Transform* transform) -> bool
{
    return !transform->Parent().Valid();
}
} // anonymous namespace

namespace nc::ui::editor
{
void SceneGraph::Draw(Registry* registry)
{
    EnsureSelection(registry);
    m_tagFilter.Draw("search");

    ChildWindow("Entity List", [&]()
    {
        if (IsWindowBackgroundClicked())
        {
            m_selectedEntity = Entity::Null();
        }

        GraphContextMenu(registry);
        Graph(registry);
    });
}

void SceneGraph::EnsureSelection(Registry* registry)
{
    if (m_selectedEntity.Valid() && !registry->Contains<Entity>(m_selectedEntity))
    {
        m_selectedEntity = Entity::Null();
    }
}

auto SceneGraph::PassFilter(Tag& tag) -> bool
{
    return m_tagFilter.PassFilter(tag.Value().data());
}

void SceneGraph::Graph(Registry* registry)
{
    for(auto entity : View<Entity>{registry})
    {
        auto transform = registry->Get<Transform>(entity);
        auto tag = registry->Get<Tag>(entity);
        if (IsRoot(transform) && PassFilter(*tag))
        {
            GraphNode(registry, entity, *tag, *transform);
        }
    }
}

void SceneGraph::GraphNode(Registry* registry, Entity entity, Tag& tag, Transform& transform)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = m_selectedEntity == entity ? ImGuiTreeNodeFlags_Framed : 0;
    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if(ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
    }

    NodeContextMenu(registry, entity);

    DragAndDropSource<Entity>(&entity);
    DragAndDropTarget<Entity>([entity, registry](Entity* source)
    {
        registry->Get<Transform>(*source)->SetParent(entity);
    });

    if(isNodeExpanded)
    {
        for(auto child : transform.Children())
        {
            GraphNode(registry, child, *registry->Get<Tag>(child), *registry->Get<Transform>(child));
        }

        ImGui::TreePop();
    }
}

void SceneGraph::GraphContextMenu(Registry* registry)
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("New Entity"))
            registry->Add<Entity>(EntityInfo{});
        else if (ImGui::Selectable("New Static Entity"))
            registry->Add<Entity>(EntityInfo{.flags = Entity::Flags::Static});
        else
            m_selectedEntity = Entity::Null();

        ImGui::EndPopup();
    }
}

void SceneGraph::NodeContextMenu(Registry* registry, Entity entity)
{
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Selectable("Add Child"))
            m_selectedEntity = registry->Add<Entity>(EntityInfo{.parent = entity});
        else if (ImGui::Selectable("Make Root"))
            registry->Get<Transform>(entity)->SetParent(Entity::Null());
        else
            m_selectedEntity = entity;

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
