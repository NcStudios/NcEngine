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
void SceneGraph::Draw(EditorContext& ctx, CreateEntityDialog& createEntity)
{
    {
        static bool _ = [&]()
        {
            auto& world = ctx.world;
            // TODO: move this to c'tor once other PR is merged
            // TODO: should go in 'Internal' bucket
            // TODO: clear selected entity on editor close, or at least clear wireframe
            m_selectedEntityWireframe = world.Emplace<Entity>({
                .parent = ctx.objectBucket,
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
                .parent = ctx.objectBucket,
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

    EnsureSelection(ctx);
    m_tagFilter.Draw("search");

    ChildWindow("Entity List", [&]()
    {
        if (IsWindowBackgroundClicked())
        {
            SetEntitySelection(ctx, Entity::Null());
        }

        GraphContextMenu(ctx, createEntity);
        Graph(ctx, createEntity);
    });
}

void SceneGraph::OnClose(EditorContext& ctx)
{
    SetEntitySelection(ctx, Entity::Null());
}

void SceneGraph::SetEntitySelection(EditorContext& ctx, Entity entity)
{
    ctx.selectedEntity = entity;
    ctx.world.Get<graphics::WireframeRenderer>(m_selectedEntityWireframe).target = entity;
    ctx.world.Get<graphics::WireframeRenderer>(m_selectedColliderWireframe).target =
        (entity.Valid() && ctx.world.Contains<physics::Collider>(entity))
        ? entity
        : Entity::Null();
}

void SceneGraph::EnsureSelection(EditorContext& ctx)
{
    if (ctx.selectedEntity.Valid() && !ctx.world.Contains<Transform>(ctx.selectedEntity))
    {
        SetEntitySelection(ctx, Entity::Null());
    }
}

auto SceneGraph::PassFilter(Tag& tag) -> bool
{
    return m_tagFilter.PassFilter(tag.Value().data());
}

void SceneGraph::Graph(EditorContext& ctx, CreateEntityDialog& createEntity)
{
    for(auto entity : ctx.world.GetAll<Entity>())
    {
        auto& transform = ctx.world.Get<Transform>(entity);
        auto& tag = ctx.world.Get<Tag>(entity);
        if (IsRoot(transform) && PassFilter(tag))
        {
            GraphNode(ctx, entity, tag, transform, createEntity);
        }
    }
}

void SceneGraph::GraphNode(EditorContext& ctx, Entity entity, Tag& tag, Transform& transform, CreateEntityDialog& createEntity)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = ctx.selectedEntity == entity
        ? ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow
        : 0;

    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if (ImGui::IsItemClicked())
    {
        SetEntitySelection(ctx, entity);
    }
    else if (ImGui::BeginPopupContextItem())
    {
        const auto selectedEntityFromContext = EntityContextMenu(entity, ctx.world, createEntity);
        if (selectedEntityFromContext != ctx.selectedEntity)
        {
            SetEntitySelection(ctx, selectedEntityFromContext);
        }

        ImGui::EndPopup();
    }

    DragAndDropSource<Entity>(&entity);
    DragAndDropTarget<Entity>([entity, world = ctx.world](Entity* source) mutable
    {
        world.Get<Transform>(*source).SetParent(entity);
    });

    if(isNodeExpanded)
    {
        for(auto child : transform.Children())
        {
            GraphNode(ctx, child, ctx.world.Get<Tag>(child), ctx.world.Get<Transform>(child), createEntity);
        }

        ImGui::TreePop();
    }
}

void SceneGraph::GraphContextMenu(EditorContext& ctx, CreateEntityDialog& createEntity)
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("New Entity"))
            createEntity.Open();
        else if (ImGui::Selectable("New Default Entity"))
            ctx.selectedEntity = ctx.world.Emplace<Entity>(EntityInfo{});
        else
            SetEntitySelection(ctx, Entity::Null());

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
