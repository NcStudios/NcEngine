#include "SceneGraph.h"
#include "EntityContextMenu.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
auto MakeSelectedEntityWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedEntity",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::graphics::WireframeRenderer>(
        entity,
        nc::Entity::Null(),
        nc::graphics::WireframeSource::Renderer
    );

    return entity;
}

auto MakeSelectedColliderWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedCollider",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::graphics::WireframeRenderer>(
        entity,
        nc::Entity::Null(),
        nc::graphics::WireframeSource::Collider
    );

    return entity;
}

auto IsRoot(nc::Transform& transform) -> bool
{
    return !transform.Parent().Valid();
}
} // anonymous namespace

namespace nc::ui::editor
{
SceneGraph::SceneGraph(EditorContext& ctx)
    : m_selectedEntityWireframe{::MakeSelectedEntityWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedColliderWireframe{::MakeSelectedColliderWireFrame(ctx.world, ctx.objectBucket)}
{
}

void SceneGraph::Draw(EditorContext& ctx, CreateEntityDialog& createEntity)
{
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
        world.SetParent(*source, entity);
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
        {
            createEntity.Open();
        }
        else if (ImGui::Selectable("New Default Entity"))
        {
            ctx.selectedEntity = ctx.world.Emplace<Entity>(EntityInfo{});
        }
        else if (ImGui::BeginMenu("Object"))
        {
            if (ImGui::Selectable("PointLight"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.position = Vector3{1.0f, 10.0f, -1.0f}, .tag = "PointLight"});
                ctx.world.Emplace<graphics::PointLight>(ctx.selectedEntity);
            }
            else if (ImGui::Selectable("Cube"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Cube"});
                ctx.world.Emplace<graphics::MeshRenderer>(ctx.selectedEntity, asset::CubeMesh);
            }
            else if (ImGui::Selectable("Sphere"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Sphere"});
                ctx.world.Emplace<graphics::MeshRenderer>(ctx.selectedEntity, asset::SphereMesh);
            }
            else if (ImGui::Selectable("Capsule"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Capsule"});
                ctx.world.Emplace<graphics::MeshRenderer>(ctx.selectedEntity, asset::CapsuleMesh);
            }
            else if (ImGui::Selectable("Plane"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Plane"});
                ctx.world.Emplace<graphics::MeshRenderer>(ctx.selectedEntity, asset::PlaneMesh);
            }

            ImGui::EndMenu();
        }
        else
        {
            SetEntitySelection(ctx, Entity::Null());
        }

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
