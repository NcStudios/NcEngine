#include "Benchmarks.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <functional>

namespace
{
constexpr auto g_mapExtent = 150.0f;
constexpr auto g_buttonWidth = 100.0f;
constexpr auto g_buttonHeight = 20.0f;
auto g_maxEntities = 0u;
auto g_maxRigidBodies = 0u;
auto g_maxMeshes = 0u;
auto g_maxParticleEmitters = 0u;
auto g_maxPointLights = 0u;
auto g_maxSpotLights = 0u;
auto g_maxHierarchies = 0u;
auto g_currentEntities = 0u;
auto g_currentRigidBodies = 0u;
auto g_currentStaticMeshes = 0u;
auto g_currentSkinnedMeshes = 0u;
auto g_currentParticleEmitters = 0u;
auto g_currentPointLights = 0u;
auto g_currentSpotLights = 0u;
auto g_currentHierarchies = 0u;

constexpr auto g_assets = std::array{
    std::string_view{nc::asset::CubeMesh},
    std::string_view{nc::asset::SphereMesh},
    std::string_view{nc::asset::CapsuleMesh},
    std::string_view{nc::sample::mesh::path::ramp},
    std::string_view{nc::sample::mesh::path::halfpipe}
};

const auto g_meshViews = std::array{
    &nc::sample::mesh::Cube,
    &nc::sample::mesh::Sphere,
    &nc::sample::mesh::Capsule,
    &nc::sample::mesh::ramp,
    &nc::sample::mesh::halfpipe
};

// Need to store ptrs b/c deferred initialization
const auto g_materials = std::array{
    &nc::sample::material::Default,
    &nc::sample::material::Red,
    &nc::sample::material::Green,
    &nc::sample::material::Blue,
    &nc::sample::material::Orange,
    &nc::sample::material::Purple,
    &nc::sample::material::Teal,
    &nc::sample::material::Yellow
};

auto MeshFromPath(std::string_view path) -> const nc::asset::MeshView&
{
    const auto pos = std::ranges::find(g_assets, path);
    NC_ASSERT(pos != g_assets.end(), "Mesh not found");
    const auto index = std::distance(g_assets.begin(), pos);
    return *g_meshViews.at(index);
}

auto RandomMaterial() -> const nc::MaterialDesc&
{
    static auto index = 0ull;
    ++index;
    index = index % (g_materials.size() - 1);
    return *g_materials.at(index);
}

auto AssetCombo(std::string& selection) -> bool
{
    return nc::ui::Combobox(selection, "##assetcombo", g_assets);
}

auto AssetComboExcludeMeshCollider(std::string& selection) -> bool
{
    const auto disableMeshCollider = [](const auto& entry){ return entry == nc::sample::mesh::path::halfpipe; };
    return nc::ui::FilteredCombobox(selection, "##assetcombo", g_assets, disableMeshCollider);
}

auto AddRigidBodyForMesh(nc::ecs::Ecs world, nc::Entity entity, std::string_view mesh, nc::BodyType type = nc::BodyType::Dynamic) -> nc::RigidBody&
{
    auto shape = [&]()
    {
        if (mesh == nc::asset::CubeMesh)
            return nc::Shape::MakeBox();
        else if (mesh == nc::asset::SphereMesh)
            return nc::Shape::MakeSphere();
        else if (mesh == nc::asset::CapsuleMesh)
            return nc::Shape::MakeCapsule();
        else if (mesh == nc::sample::convex_hull::path::ramp)
            return nc::Shape::MakeConvexHull(nc::sample::convex_hull::ramp);
        else if (mesh == nc::sample::mesh_collider::path::halfpipe)
            return nc::Shape::MakeMesh(nc::sample::mesh_collider::halfpipe);
        else
            throw nc::NcError(fmt::format("Unexpected mesh '{}'", mesh));
    }();

    return world.Emplace<nc::RigidBody>(
        entity,
        shape,
        nc::RigidBodyInfo{
            .type = type
        }
    );
}

struct static_mesh
{
    static constexpr auto name = "Static Mesh";
    static inline const auto& maxCount = g_maxMeshes;
    static inline auto& currentCount = g_currentStaticMeshes;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
    static inline std::string Mesh = std::string{nc::asset::CubeMesh};
};

struct skinned_mesh
{
    static constexpr auto name = "Skinned Mesh";
    static inline const auto& maxCount = g_maxMeshes;
    static inline auto& currentCount = g_currentSkinnedMeshes;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
};

struct static_body
{
    static constexpr auto name = "Static Body";
    static inline const auto& maxCount = g_maxRigidBodies;
    static inline auto& currentCount = g_currentRigidBodies;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
    static inline std::string Mesh = std::string{nc::asset::CubeMesh};
};

struct rigid_body
{
    static constexpr auto name = "Rigid Body";
    static inline const auto& maxCount = g_maxRigidBodies;
    static inline auto& currentCount = g_currentRigidBodies;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
    static inline std::string Mesh = std::string{nc::asset::CubeMesh};
};

struct point_light
{
    static constexpr auto name = "Point Light";
    static inline const auto& maxCount = g_maxPointLights;
    static inline auto& currentCount = g_currentPointLights;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
};

struct spot_light
{
    static constexpr auto name = "Spot Light";
    static inline const auto& maxCount = g_maxSpotLights;
    static inline auto& currentCount = g_currentSpotLights;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
};

struct particle_emitter
{
    static constexpr auto name = "Particle Emitter";
    static inline const auto& maxCount = g_maxParticleEmitters;
    static inline auto& currentCount = g_currentParticleEmitters;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
};

struct entity_hierarchy
{
    static constexpr auto name = "Entity Hierarchy";
    static inline const auto& maxCount = g_maxHierarchies;
    static inline auto& currentCount = g_currentHierarchies;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
    static inline unsigned HierarchySize = 200;

    static void Rotate(nc::Entity self, nc::ecs::Ecs world, float dt)
    {
        auto& transform = world.Get<nc::Transform>(self);
        transform.Rotate(nc::Vector3::Up(), 0.3f * dt);
    }

    static void AttachChildren(nc::ecs::Ecs world, nc::Entity root)
    {
        auto parent = root;
        auto count = HierarchySize;
        while (count-- > 0)
        {
            const auto child = world.Emplace<nc::Entity>({
                .position = nc::Vector3::Up(),
                .rotation = nc::Quaternion::FromAxisAngle(nc::Vector3::Up(), 0.05f),
                .parent = parent
            });

            world.Emplace<nc::StaticMesh>(
                child,
                MeshFromPath(nc::asset::CubeMesh),
                RandomMaterial()
            );
            parent = child;
        }
    }
};

template<class T>
struct InnerWidget
{
    void operator()(float buttonWidth, auto&& extension = [](){})
    {
        IMGUI_SCOPE(nc::ui::ImGuiId, T::name);
        const auto& currentObjectCount = T::currentCount;
        const auto remainingEntityCount = g_maxEntities - g_currentEntities;
        const auto maxObjectCount = nc::Min(T::maxCount - currentObjectCount, remainingEntityCount);

        ImGui::Spacing();
        ImGui::Text("%s (%u)", T::name, currentObjectCount);

        ImGui::SetNextItemWidth(buttonWidth);
        nc::ui::InputU32(T::SpawnCount, "##spawncount");
        T::SpawnCount = nc::Clamp(T::SpawnCount, 0u, maxObjectCount);

        ImGui::SameLine();

        ImGui::SetNextItemWidth(buttonWidth);
        nc::ui::InputU32(T::DestroyCount, "##destroycount");

        if(ImGui::Button("Spawn", {buttonWidth, 0}))
        {
            T::SpawnCallback(T::SpawnCount);
            T::currentCount += T::SpawnCount;
            g_currentEntities += T::SpawnCount;
        }

        ImGui::SameLine();

        if(ImGui::Button("Destroy", {buttonWidth, 0}))
        {
            T::DestroyCallback(T::DestroyCount);
            const auto actualDestroyed = nc::Min(T::DestroyCount, currentObjectCount);
            T::currentCount -= actualDestroyed;
            g_currentEntities -= actualDestroyed;
        }

        extension();
        ImGui::Spacing();
    }
};

template<>
struct InnerWidget<entity_hierarchy>
{
    void operator()(float buttonWidth, auto&& extension = [](){})
    {
        using T = entity_hierarchy;
        IMGUI_SCOPE(nc::ui::ImGuiId, T::name);

        const auto objectsPerSpawn = 1 + entity_hierarchy::HierarchySize;
        g_maxHierarchies = g_maxEntities == 0 ? 0 : g_maxEntities / objectsPerSpawn;
        const auto& currentObjectCount = T::currentCount;
        const auto remainingEntityCount = g_maxEntities - g_currentEntities;
        const auto maxObjectCount = nc::Min(g_maxHierarchies - currentObjectCount, remainingEntityCount / objectsPerSpawn);

        ImGui::Spacing();
        ImGui::Text("%s (%u)", T::name, currentObjectCount);

        ImGui::SetNextItemWidth(buttonWidth);
        nc::ui::InputU32(T::SpawnCount, "##spawncount");
        T::SpawnCount = nc::Clamp(T::SpawnCount, 0u, maxObjectCount);

        ImGui::SameLine();

        ImGui::SetNextItemWidth(buttonWidth);
        nc::ui::InputU32(T::DestroyCount, "##destroycount");

        if(ImGui::Button("Spawn", {buttonWidth, 0}))
        {
            T::SpawnCallback(T::SpawnCount);
            T::currentCount += T::SpawnCount;
            g_currentEntities += objectsPerSpawn * T::SpawnCount;
        }

        ImGui::SameLine();

        if(ImGui::Button("Destroy", {buttonWidth, 0}))
        {
            T::DestroyCallback(T::DestroyCount);
            const auto actualDestroyed = nc::Min(T::DestroyCount, currentObjectCount);
            T::currentCount -= actualDestroyed;
            g_currentEntities -= (actualDestroyed * objectsPerSpawn);
        }

        extension();
        ImGui::Spacing();
    }
};

void Widget()
{
    ImGui::Text("Objects: %u (max: %u)", g_currentEntities, g_maxEntities);
    nc::ui::ChildWindow("Benchmarks", []()
    {
        const auto [cellWidth, halfCellWidth] = []()
        {
            const auto columnWidth = (ImGui::GetColumnWidth() * 0.5f);
            return ImVec2{columnWidth - 10.0f, (columnWidth * 0.5f) - 10.0f};
        }();

        constexpr auto flags = ImGuiTableFlags_Borders;
        if (ImGui::BeginTable("table", 2, flags))
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<static_mesh>{}(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(static_mesh::Mesh);
            });

            ImGui::TableNextColumn();
            InnerWidget<skinned_mesh>{}(halfCellWidth, [](){});

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<static_body>{}(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(static_body::Mesh);
            });

            ImGui::TableNextColumn();
            InnerWidget<rigid_body>{}(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetComboExcludeMeshCollider(rigid_body::Mesh);
            });

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<point_light>{}(halfCellWidth, [](){});

            ImGui::TableNextColumn();
            InnerWidget<spot_light>{}(halfCellWidth, [](){});

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<entity_hierarchy>{}(halfCellWidth, [halfCellWidth](){
                ImGui::SetNextItemWidth(halfCellWidth);
                nc::ui::InputU32(entity_hierarchy::HierarchySize, "Hierarchy Size");
            });

            ImGui::TableNextColumn();
            InnerWidget<particle_emitter>{}(halfCellWidth, [](){});

            ImGui::EndTable();
        }
    });
}
} // anonymous namespace

namespace nc::sample
{
Benchmarks::Benchmarks(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void Benchmarks::Load(ecs::Ecs world, ModuleProvider modules)
{
    ReloadPrefabs();

    {
        // maxes need to account for objects in scene created outside of spawner (ground + light)
        const auto& config = config::GetMemorySettings();
        ::g_maxEntities = config.maxTransforms;
        ::g_maxRigidBodies = config.maxRigidBodies - 1;
        ::g_maxMeshes = config.maxRenderers - 1;
        ::g_maxParticleEmitters = config.maxParticleEmitters;
        ::g_maxPointLights = config.maxPointLights - 1;
        ::g_maxSpotLights = config.maxSpotLights;
        ::g_maxHierarchies = ::g_maxEntities / (entity_hierarchy::SpawnCount + 1);
    }

    m_sampleUI->SetWidgetCallback(::Widget);
    auto ncGraphics = modules.Get<NcGraphics>();
    auto ncRandom = modules.Get<Random>();

    ncGraphics->SetSkybox(asset::DefaultSkyboxCubeMap);

    world.Emplace<PointLight>(
        world.Emplace<Entity>({
            .position = Vector3{0.0f, 41.0f, -12.0f},
            .tag = "Point Light"
        }),
        Vector3{1.0f, 0.871f, 0.6f},
        Vector3{1.0f, 0.871f, 0.6f},
        10.0f,
        500.0f
    );

    const auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.0f, 35.0f, -g_mapExtent * 0.8f},
        .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<SceneNavigationCamera>{});
    ncGraphics->SetCamera(&camera);

    auto ground = world.Emplace<Entity>({
        .position = Vector3{0.0f, -1.0f, 0.0f},
        .scale = Vector3{g_mapExtent, 1.0f, g_mapExtent},
        .tag = "Ground",
        .flags = Entity::Flags::Static
    });

    world.Emplace<StaticMesh>(ground, mesh::Cube, material::Blue);
    world.Emplace<RigidBody>(ground, Shape::MakeBox());

    const auto spawnBehavior = SpawnBehavior{
        .minPosition = Vector3{g_mapExtent * -0.4f, 1.0f, g_mapExtent * -0.4f},
        .maxPosition = Vector3{g_mapExtent * 0.4f, 30.0f, g_mapExtent * 0.4f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    // Static Mesh
    {
        const auto handle = world.Emplace<Entity>({.tag = "StaticMesh Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<StaticMesh>(
                    entity,
                    MeshFromPath(::static_mesh::Mesh),
                    ::RandomMaterial()
                );
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::static_mesh::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::static_mesh::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::static_mesh::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Skinned Mesh
    {
        const auto handle = world.Emplace<Entity>({.tag = "SkinnedMesh Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<SkinnedMesh>(
                    entity,
                    mesh::ogre,
                    material::Ogre,
                    animation::ogre_idle
                );
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::skinned_mesh::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::skinned_mesh::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::skinned_mesh::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Static Rigid Body
    {
        const auto handle = world.Emplace<Entity>({.tag = "Collider Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<StaticMesh>(
                    entity,
                    MeshFromPath(::static_body::Mesh),
                    ::RandomMaterial()
                );

                ::AddRigidBodyForMesh(world, entity, ::static_body::Mesh, BodyType::Static);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::static_body::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::static_body::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::static_body::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Rigid Body
    {
        const auto handle = world.Emplace<Entity>({.tag = "PhysicsBody Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable {
                world.Emplace<StaticMesh>(
                    entity,
                    MeshFromPath(::rigid_body::Mesh),
                    ::RandomMaterial()
                );

                ::AddRigidBodyForMesh(world, entity, ::rigid_body::Mesh);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::rigid_body::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::rigid_body::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::rigid_body::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Point Light
    {
        const auto handle = world.Emplace<Entity>({.tag = "PointLight Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable {
                world.Emplace<PointLight>(entity, Vector3{1.0f, 0.871f, 0.6f}, Vector3{1.0f, 0.871f, 0.6f}, 1.0f, 50.0f);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::point_light::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::point_light::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::point_light::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Spot Light
    {
        const auto handle = world.Emplace<Entity>({.tag = "SpotLight Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable {
                world.Emplace<SpotLight>(entity);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::spot_light::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::spot_light::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::spot_light::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Particle Emitter
    {
        const auto handle = world.Emplace<Entity>({.tag = "ParticleEmitter Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable {
                world.Emplace<ParticleEmitter>(
                    entity,
                    asset::AcquireTextureAsset(asset::DefaultParticle),
                    ParticleInfo{
                    .emission = {
                        .periodicEmissionCount = 15u,
                        .periodicEmissionFrequency = 0.1f
                    },
                    .init = {},
                    .kinematic = {
                        .velocityMin = Vector3::One() * -2.0f,
                        .velocityMax = Vector3::One() * 2.0f,
                        .scaleOverTimeFactor = -10.0f
                    }
                });
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::particle_emitter::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::particle_emitter::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::particle_emitter::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Entity Hierarchy
    {
        const auto handle = world.Emplace<Entity>({.tag = "Hierarchy Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            SpawnBehavior{
                .minPosition = Vector3{g_mapExtent * -0.4f, 0.0f, g_mapExtent * -0.4f},
                .maxPosition = Vector3{g_mapExtent * 0.4f, 0.0f, g_mapExtent * 0.4f}
            },
            [world](Entity entity) mutable{
                world.Emplace<StaticMesh>(
                    entity,
                    MeshFromPath(asset::CubeMesh),
                    ::RandomMaterial()
                );

                world.Emplace<FrameLogic>(entity, &::entity_hierarchy::Rotate);
                ::entity_hierarchy::AttachChildren(world, entity);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::entity_hierarchy::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::entity_hierarchy::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::entity_hierarchy::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Post process
    ncGraphics->SetPostProcessEffectEnabled(nc::OutlinedToonEffectId, true);
    ncGraphics->SetPostProcessEffectProperties(nc::OutlinedToonEffectId, PostProcessPassFlag::Outline, post_process::Outline);
    ncGraphics->SetPostProcessEffectProperties(nc::OutlinedToonEffectId, PostProcessPassFlag::Noise, post_process::Noise);

    g_currentEntities += static_cast<unsigned>(world.GetAll<Entity>().size());
}

void Benchmarks::Unload()
{
    g_currentEntities = 0u;
    g_currentRigidBodies = 0u;
    g_currentStaticMeshes = 0u;
    g_currentSkinnedMeshes = 0u;
    g_currentParticleEmitters = 0u;
    g_currentPointLights = 0u;
    g_currentSpotLights = 0u;
    g_currentHierarchies = 0u;
    m_sampleUI->SetWidgetCallback(nullptr);
}
} // namespace nc::sample
