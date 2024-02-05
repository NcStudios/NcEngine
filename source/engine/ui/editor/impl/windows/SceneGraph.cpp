#include "SceneGraph.h"
#include "EntityContextMenu.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
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
void SceneGraph::Draw(ecs::Ecs world)
{
    {
        static bool _ = [&]()
        {
            // TODO: move this to c'tor once other PR is merged
            // TODO: should go in 'Internal' bucket
            // TODO: clear selected entity on editor close, or at least clear wireframe
            m_selectedEntityWireframe = world.Emplace<Entity>({
                .tag = "SelectedEntity",
                .flags = Entity::Flags::Persistent |
                         Entity::Flags::Internal |
                         Entity::Flags::NoSerialize
            });

            world.Emplace<graphics::WireframeRenderer>(
                m_selectedEntityWireframe,
                Entity::Null(),
                graphics::WireframeSource::Renderer
            );

            m_selectedColliderWireframe = world.Emplace<Entity>({
                .tag = "SelectedCollider",
                .flags = Entity::Flags::Persistent |
                         Entity::Flags::Internal |
                         Entity::Flags::NoSerialize
            });

            world.Emplace<graphics::WireframeRenderer>(
                m_selectedColliderWireframe,
                Entity::Null(),
                graphics::WireframeSource::Collider
            );

            return true;
        }();
    }

    EnsureSelection(world);
    m_tagFilter.Draw("search");

    ChildWindow("Entity List", [&]()
    {
        if (IsWindowBackgroundClicked())
        {
            SetEntitySelection(world, Entity::Null());
        }

        GraphContextMenu(world);
        Graph(world);
    });
}

void SceneGraph::OnClose(ecs::Ecs world)
{
    SetEntitySelection(world, Entity::Null());
}

void SceneGraph::SetEntitySelection(ecs::Ecs world, Entity entity)
{
    m_selectedEntity = entity;
    world.Get<graphics::WireframeRenderer>(m_selectedEntityWireframe).target = entity;
    world.Get<graphics::WireframeRenderer>(m_selectedColliderWireframe).target =
        (entity.Valid() && world.Contains<physics::Collider>(entity))
        ? entity
        : Entity::Null();
}

void SceneGraph::EnsureSelection(ecs::Ecs world)
{
    if (m_selectedEntity.Valid() && !world.Contains<Transform>(m_selectedEntity))
    {
        SetEntitySelection(world, Entity::Null());
    }
}

auto SceneGraph::PassFilter(Tag& tag) -> bool
{
    return m_tagFilter.PassFilter(tag.Value().data());
}

void SceneGraph::Graph(ecs::Ecs world)
{
    for(auto entity : world.GetAll<Entity>())
    {
        auto& transform = world.Get<Transform>(entity);
        auto& tag = world.Get<Tag>(entity);
        if (IsRoot(transform) && PassFilter(tag))
        {
            GraphNode(world, entity, tag, transform);
        }
    }
}

void SceneGraph::GraphNode(ecs::Ecs world, Entity entity, Tag& tag, Transform& transform)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = m_selectedEntity == entity ? ImGuiTreeNodeFlags_Framed : 0;
    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if (ImGui::IsItemClicked())
    {
        SetEntitySelection(world, entity);
    }
    else if (ImGui::BeginPopupContextItem())
    {
        const auto selectedEntityFromContext = EntityContextMenu(entity, world);
        if (selectedEntityFromContext != m_selectedEntity)
        {
            SetEntitySelection(world, selectedEntityFromContext);
        }

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
            GraphNode(world, child, world.Get<Tag>(child), world.Get<Transform>(child));
        }

        ImGui::TreePop();
    }
}

void SceneGraph::GraphContextMenu(ecs::Ecs world)
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("New Entity"))
            SetEntitySelection(world, world.Emplace<Entity>(EntityInfo{}));
        else if (ImGui::Selectable("New Static Entity"))
            SetEntitySelection(world, world.Emplace<Entity>(EntityInfo{.flags = Entity::Flags::Static}));
        else
            SetEntitySelection(world, Entity::Null());

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
