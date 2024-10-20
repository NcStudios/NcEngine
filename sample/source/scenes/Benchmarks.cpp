#include "Benchmarks.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/NcGraphics.h"
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
auto g_maxPointLights = 0u;
auto g_maxSpotLights = 0u;
auto g_currentEntities = 0u;

constexpr auto g_assets = std::array{
    std::string_view{nc::asset::CubeMesh},
    std::string_view{nc::asset::SphereMesh},
    std::string_view{nc::asset::CapsuleMesh},
    std::string_view{nc::sample::RampMesh}
};

// Need to store ptrs b/c deferred initialization
const auto g_pbrMaterials = std::array{
    &nc::sample::DefaultPbrMaterial,
    &nc::sample::RedPbrMaterial,
    &nc::sample::GreenPbrMaterial,
    &nc::sample::BluePbrMaterial,
    &nc::sample::OrangePbrMaterial,
    &nc::sample::PurplePbrMaterial,
    &nc::sample::TealPbrMaterial,
    &nc::sample::YellowPbrMaterial
};

const auto g_toonMaterials = std::array{
    &nc::sample::DefaultToonMaterial,
    &nc::sample::RedToonMaterial,
    &nc::sample::GreenToonMaterial,
    &nc::sample::BlueToonMaterial,
    &nc::sample::OrangeToonMaterial,
    &nc::sample::PurpleToonMaterial,
    &nc::sample::TealToonMaterial,
    &nc::sample::YellowToonMaterial
};

auto RandomPbrMaterial() -> const nc::graphics::PbrMaterial&
{
    static auto index = 0ull;
    ++index;
    index = index % (g_pbrMaterials.size() - 1);
    return *g_pbrMaterials.at(index);
}

auto RandomToonMaterial() -> const nc::graphics::ToonMaterial&
{
    static auto index = 0ull;
    ++index;
    index = index % (g_toonMaterials.size() - 1);
    return *g_toonMaterials.at(index);
}

auto AssetCombo(std::string& selection) -> bool
{
    return nc::ui::Combobox(selection, "##assetcombo", g_assets);
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
        else if (mesh == nc::sample::RampMesh) // todo: #693
            return nc::Shape::MakeBox();
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

struct mesh_renderer
{
    static constexpr auto name = "Mesh Renderer";
    static inline const auto& maxCount = g_maxEntities;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
    static inline std::string Mesh = std::string{nc::asset::CubeMesh};
};

struct toon_renderer
{
    static constexpr auto name = "Toon Renderer";
    static inline const auto& maxCount = g_maxEntities;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1000;
    static inline unsigned DestroyCount = 1000;
    static inline std::string Mesh = std::string{nc::asset::CubeMesh};
};

struct static_body
{
    static constexpr auto name = "Static Body";
    static inline const auto& maxCount = g_maxEntities;
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
    static inline const auto& maxCount = g_maxEntities;
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
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
};

struct particle_emitter
{
    static constexpr auto name = "Particle Emitter";
    static inline const auto& maxCount = g_maxEntities;
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline unsigned SpawnCount = 1;
    static inline unsigned DestroyCount = 1;
};

struct entity_hierarchy
{
    static constexpr auto name = "Entity Hierarchy";
    static inline const auto& maxCount = g_maxEntities;
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

            world.Emplace<nc::graphics::MeshRenderer>(child, nc::asset::CubeMesh, RandomPbrMaterial());
            parent = child;
        }
    }
};

template<class T>
void InnerWidget(float buttonWidth, auto&& extension = [](){})
{
    IMGUI_SCOPE(nc::ui::ImGuiId, T::name);
    const auto currentObjectCount = static_cast<unsigned>(T::GetObjectCountCallback());
    const auto remainingEntityCount = g_maxEntities - g_currentEntities;
    const auto maxObjectCount = nc::Min(T::maxCount - currentObjectCount, remainingEntityCount);

    ImGui::Spacing();
    ImGui::Text("%s", T::name);

    ImGui::SetNextItemWidth(buttonWidth);
    nc::ui::InputU32(T::SpawnCount, "##spawncount");
    T::SpawnCount = nc::Clamp(T::SpawnCount, 0u, maxObjectCount);

    ImGui::SameLine();

    ImGui::SetNextItemWidth(buttonWidth);
    nc::ui::InputU32(T::DestroyCount, "##destroycount");

    if(ImGui::Button("Spawn", {buttonWidth, 0}))
    {
        T::SpawnCallback(T::SpawnCount);
        g_currentEntities += T::SpawnCount;
    }

    ImGui::SameLine();

    if(ImGui::Button("Destroy", {buttonWidth, 0}))
    {
        T::DestroyCallback(T::DestroyCount);
        g_currentEntities -= nc::Min(T::DestroyCount, currentObjectCount);
    }

    extension();
    ImGui::Spacing();
}

void Widget()
{
    ImGui::Text("Objects: %u", g_currentEntities);
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
            InnerWidget<mesh_renderer>(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(mesh_renderer::Mesh);
            });

            ImGui::TableNextColumn();
            InnerWidget<toon_renderer>(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(toon_renderer::Mesh);
            });

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<static_body>(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(static_body::Mesh);
            });

            ImGui::TableNextColumn();
            InnerWidget<rigid_body>(halfCellWidth, [cellWidth](){
                ImGui::SetNextItemWidth(cellWidth);
                AssetCombo(static_body::Mesh);
            });

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<point_light>(halfCellWidth, [](){});

            ImGui::TableNextColumn();
            InnerWidget<spot_light>(halfCellWidth, [](){});

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            InnerWidget<entity_hierarchy>(halfCellWidth, [halfCellWidth](){
                ImGui::SetNextItemWidth(halfCellWidth);
                nc::ui::InputU32(entity_hierarchy::HierarchySize, "Hierarchy Size");
            });

            ImGui::TableNextColumn();
            InnerWidget<particle_emitter>(halfCellWidth, [](){});

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
    {
        const auto& config = config::GetMemorySettings();
        ::g_maxEntities = config.maxTransforms;
        ::g_maxPointLights = config.maxPointLights - 1u;
        ::g_maxSpotLights = config.maxSpotLights - 1u;
    }

    m_sampleUI->SetWidgetCallback(::Widget);
    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    auto ncRandom = modules.Get<Random>();

    ncGraphics->SetSkybox(asset::DefaultSkyboxCubeMap);

    world.Emplace<graphics::PointLight>(
        world.Emplace<Entity>({
            .position = Vector3{0.0f, 41.0f, -12.0f},
            .tag = "Point Light"
        }),
        Vector3{1.0f, 0.871f, 0.6f},
        Vector3{1.0f, 0.871f, 0.6f},
        113.0f
    );

    const auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.0f, 35.0f, -g_mapExtent * 0.8f},
        .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<graphics::SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    ncGraphics->SetCamera(&camera);

    auto ground = world.Emplace<Entity>({
        .position = Vector3{0.0f, -1.0f, 0.0f},
        .scale = Vector3{g_mapExtent, 1.0f, g_mapExtent},
        .tag = "Ground",
        .flags = Entity::Flags::Static
    });

    world.Emplace<graphics::ToonRenderer>(ground, asset::CubeMesh, BlueToonMaterial);
    world.Emplace<RigidBody>(ground, Shape::MakeBox());

    const auto spawnBehavior = SpawnBehavior{
        .minPosition = Vector3{g_mapExtent * -0.4f, 1.0f, g_mapExtent * -0.4f},
        .maxPosition = Vector3{g_mapExtent * 0.4f, 30.0f, g_mapExtent * 0.4f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    // Mesh Renderer
    {
        const auto handle = world.Emplace<Entity>({.tag = "MeshRenderer Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<graphics::MeshRenderer>(entity, ::mesh_renderer::Mesh, ::RandomPbrMaterial());
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::mesh_renderer::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::mesh_renderer::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::mesh_renderer::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Toon Renderer
    {
        const auto handle = world.Emplace<Entity>({.tag = "ToonRenderer Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<graphics::ToonRenderer>(entity, ::toon_renderer::Mesh, ::RandomToonMaterial());
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::toon_renderer::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::toon_renderer::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::toon_renderer::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Static Rigid Body
    {
        const auto handle = world.Emplace<Entity>({.tag = "Collider Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            spawnBehavior,
            [world](Entity entity) mutable{
                world.Emplace<graphics::MeshRenderer>(entity, ::static_body::Mesh, ::RandomPbrMaterial());
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
                world.Emplace<graphics::ToonRenderer>(entity, ::rigid_body::Mesh, ::RandomToonMaterial());
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
                world.Emplace<graphics::PointLight>(entity, Vector3{1.0f, 0.871f, 0.6f}, Vector3{1.0f, 0.871f, 0.6f}, 50.0f);
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
                world.Emplace<graphics::SpotLight>(entity);
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
                world.Emplace<graphics::ParticleEmitter>(entity, graphics::ParticleInfo{
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
                world.Emplace<graphics::MeshRenderer>(entity, asset::CubeMesh, ::RandomPbrMaterial());
                world.Emplace<FrameLogic>(entity, &::entity_hierarchy::Rotate);
                ::entity_hierarchy::AttachChildren(world, entity);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::entity_hierarchy::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::entity_hierarchy::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::entity_hierarchy::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    g_currentEntities += static_cast<unsigned>(world.GetAll<Entity>().size());
}

void Benchmarks::Unload()
{
    g_currentEntities = 0u;
    m_sampleUI->SetWidgetCallback(nullptr);
}
} // namespace nc::sample
