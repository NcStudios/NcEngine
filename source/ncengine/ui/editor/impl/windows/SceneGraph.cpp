#include "SceneGraph.h"
#include "EntityContextMenu.h"

#include "ncasset/DefaultAssets.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/RigidBody.h"
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

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::Renderer,
        nc::Entity::Null(),
        nc::Vector4{1.0f, 0.0f, 0.0f, 1.0f}
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

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::Collider,
        nc::Entity::Null(),
        nc::Vector4{0.0f, 1.0f, 0.0f, 1.0f}
    );

    return entity;
}

auto MakeSelectedCameraWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedCamera",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::Camera,
        nc::Entity::Null(),
        nc::Vector4{1.0f, 1.0f, 0.0f, 1.0f}  // Yellow for camera frustum
    );

    return entity;
}

auto MakeSelectedSpotLightWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedSpotLight",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::SpotLight,
        nc::Entity::Null(),
        nc::Vector4{1.0f, 0.5f, 0.0f, 1.0f}  // Orange for spot light
    );

    return entity;
}

auto MakeSelectedPointLightWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedPointLight",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::PointLight,
        nc::Entity::Null(),
        nc::Vector4{0.0f, 1.0f, 1.0f, 1.0f}  // Cyan for point light
    );

    return entity;
}

auto MakeSelectedDirectionalLightWireFrame(nc::ecs::Ecs world, nc::Entity parent) -> nc::Entity
{
    const auto entity = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = "SelectedDirectionalLight",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::WireframeRenderer>(
        entity,
        nc::WireframeSource::DirectionalLight,
        nc::Entity::Null(),
        nc::Vector4{1.0f, 1.0f, 0.5f, 1.0f}  // Light yellow for directional light
    );

    return entity;
}
} // anonymous namespace

namespace nc::ui::editor
{
SceneGraph::SceneGraph(EditorContext& ctx)
    : m_selectedEntityWireframe{::MakeSelectedEntityWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedColliderWireframe{::MakeSelectedColliderWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedCameraWireframe{::MakeSelectedCameraWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedSpotLightWireframe{::MakeSelectedSpotLightWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedPointLightWireframe{::MakeSelectedPointLightWireFrame(ctx.world, ctx.objectBucket)},
      m_selectedDirectionalLightWireframe{::MakeSelectedDirectionalLightWireFrame(ctx.world, ctx.objectBucket)}
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
    ctx.world.Get<WireframeRenderer>(m_selectedEntityWireframe).target = entity;
    ctx.world.Get<WireframeRenderer>(m_selectedColliderWireframe).target =
        (entity.Valid() && ctx.world.Contains<RigidBody>(entity))
        ? entity
        : Entity::Null();

    const bool isCamera = entity.Valid() && ctx.world.Contains<Camera>(entity);
    ctx.world.Get<WireframeRenderer>(m_selectedCameraWireframe).target = isCamera ? entity : Entity::Null();

    // Set the selected camera entity in graphics for debug visualizations (e.g., CSM cascade colors)
    if (auto* graphics = ctx.modules.Get<NcGraphics>())
    {
        graphics->SetSelectedCameraEntity(isCamera ? entity : Entity::Null());
    }

    ctx.world.Get<WireframeRenderer>(m_selectedSpotLightWireframe).target =
        (entity.Valid() && ctx.world.Contains<SpotLight>(entity))
        ? entity
        : Entity::Null();
    ctx.world.Get<WireframeRenderer>(m_selectedPointLightWireframe).target =
        (entity.Valid() && ctx.world.Contains<PointLight>(entity))
        ? entity
        : Entity::Null();
    ctx.world.Get<WireframeRenderer>(m_selectedDirectionalLightWireframe).target =
        (entity.Valid() && ctx.world.Contains<DirectionalLight>(entity))
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
    return m_tagFilter.PassFilter(tag.value.c_str());
}

void SceneGraph::Graph(EditorContext& ctx, CreateEntityDialog& createEntity)
{
    for(auto entity : std::views::reverse(ctx.world.GetAll<Entity>()))
    {
        auto& hierarchy = ctx.world.Get<Hierarchy>(entity);
        auto& tag = ctx.world.Get<Tag>(entity);
        if (!hierarchy.parent.Valid() && PassFilter(tag))
        {
            GraphNode(ctx, entity, tag, hierarchy, createEntity);
        }
    }
}

void SceneGraph::GraphNode(EditorContext& ctx, Entity entity, Tag& tag, Hierarchy& hierarchy, CreateEntityDialog& createEntity)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = ctx.selectedEntity == entity
        ? ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow
        : 0;

    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.value.c_str(), flags);

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
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
        for(auto child : hierarchy.children)
        {
            GraphNode(ctx, child, ctx.world.Get<Tag>(child), ctx.world.Get<Hierarchy>(child), createEntity);
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
            if (ImGui::Selectable("DirectionalLight"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.position = Vector3{0.0f, 10.0f, 0.0f}, .rotation = nc::Quaternion::FromEulerAngles(1.5f, 0.809f, 0.0f), .tag = "DirectionalLight"});
                ctx.world.Emplace<DirectionalLight>(ctx.selectedEntity);
            }
            if (ImGui::Selectable("PointLight"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.position = Vector3{1.0f, 10.0f, -1.0f}, .tag = "PointLight"});
                ctx.world.Emplace<PointLight>(ctx.selectedEntity);
            }
            if (ImGui::Selectable("SpotLight"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.position = Vector3{1.0f, 10.0f, -1.0f}, .tag = "SpotLight"});
                ctx.world.Emplace<SpotLight>(ctx.selectedEntity);
            }
            else if (ImGui::Selectable("Cube"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Cube"});
                ctx.world.Emplace<StaticMesh>(
                    ctx.selectedEntity,
                    asset::AcquireMeshAsset(asset::CubeMesh),
                    MaterialDesc{
                        .properties = {
                            .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
                            .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
                            .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor)
                        }
                    });
            }
            else if (ImGui::Selectable("Sphere"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Sphere"});
                ctx.world.Emplace<StaticMesh>(
                    ctx.selectedEntity,
                    asset::AcquireMeshAsset(asset::SphereMesh),
                    MaterialDesc{
                        .properties = {
                            .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
                            .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
                            .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor)
                        }
                    });
            }
            else if (ImGui::Selectable("Capsule"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Capsule"});
                ctx.world.Emplace<StaticMesh>(
                    ctx.selectedEntity,
                    asset::AcquireMeshAsset(asset::CapsuleMesh),
                    MaterialDesc{
                        .properties = {
                            .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
                            .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
                            .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor)
                        }
                    });
            }
            else if (ImGui::Selectable("Plane"))
            {
                ctx.selectedEntity = ctx.world.Emplace<Entity>({.tag = "Plane"});
                ctx.world.Emplace<StaticMesh>(
                    ctx.selectedEntity,
                    asset::AcquireMeshAsset(asset::PlaneMesh),
                    MaterialDesc{
                        .properties = {
                            .diffuseTex = asset::AcquireTextureAsset(asset::DefaultBaseColor),
                            .normalTex = asset::AcquireTextureAsset(asset::DefaultNormal),
                            .hatchTex = asset::AcquireTextureAsset(asset::DefaultBaseColor)
                        }
                    });
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
