#include "SceneGraph.h"

#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"


#include "../editor/ui/ImGuiUtility.h"
#include "imgui/imgui.h"

namespace
{
auto IsRoot(nc::Transform* transform) -> bool
{
    return !transform->Parent().Valid();
}

// TODO: move somewhere
auto ClickedWindowBackground() -> bool
{
    return ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered();
}

// TODO: move somewhere
template<class F>
void ChildWindow(const char* label, F&& func)
{
    if (ImGui::BeginChild(label, {0, 0}, true))
    {
        func();
    };

    ImGui::EndChild();
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
        if (ClickedWindowBackground())
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
    IMGUI_SCOPE(nc::editor::ImGuiId, entity.Index());
    const auto flags = m_selectedEntity == entity ? ImGuiTreeNodeFlags_Framed : 0;
    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if(ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
    }

    NodeContextMenu(registry, entity);

    nc::editor::DragAndDropSource<Entity>(&entity);
    nc::editor::DragAndDropTarget<Entity>([entity, registry](Entity* source)
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

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
