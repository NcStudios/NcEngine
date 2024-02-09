#include "Benchmarks.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <functional>

namespace
{
struct entity_hierarchy
{
    static constexpr auto name = "Entity Hierarchy";
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline int SpawnCount = 1;
    static inline int DestroyCount = 1;
};

struct mesh_renderer
{
    static constexpr auto name = "Mesh Renderer";
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline int SpawnCount = 1000;
    static inline int DestroyCount = 1000;
};

struct toon_renderer
{
    static constexpr auto name = "Toon Renderer";
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline int SpawnCount = 1000;
    static inline int DestroyCount = 1000;
};

struct collider
{
    static constexpr auto name = "Collider";
    static inline std::function<int()> GetObjectCountCallback = nullptr;
    static inline std::function<void(unsigned)> SpawnCallback = nullptr;
    static inline std::function<void(unsigned)> DestroyCallback = nullptr;
    static inline int SpawnCount = 1000;
    static inline int DestroyCount = 1000;
};

constexpr auto g_buttonWidth = 100.0f;
constexpr auto g_buttonHeight = 20.0f;
constexpr auto g_maxSpawnCount = 30000;

template<class T>
void InnerWidget()
{
    IMGUI_SCOPE(nc::ui::ImGuiId, T::name);
    ImGui::Text("%s | Objects: %d", T::name, T::GetObjectCountCallback());

    ImGui::SetNextItemWidth(g_buttonWidth);
    ImGui::InputInt("##spawncount", &T::SpawnCount);
    T::SpawnCount = nc::Clamp(T::SpawnCount, 1, g_maxSpawnCount);

    ImGui::SameLine();

    ImGui::SetNextItemWidth(g_buttonWidth);
    ImGui::InputInt("##destroycount", &T::DestroyCount);
    T::DestroyCount = nc::Clamp(T::DestroyCount, 1, g_maxSpawnCount);

    if(ImGui::Button("Spawn", {g_buttonWidth, g_buttonHeight}))
        T::SpawnCallback(T::SpawnCount);

    ImGui::SameLine();

    if(ImGui::Button("Destroy", {g_buttonWidth, g_buttonHeight}))
        T::DestroyCallback(T::DestroyCount);

    nc::ui::SeparatorSpaced();
}

void Widget()
{
    ImGui::Text("Benchmarks");
    nc::ui::ChildWindow("Benchmarks", []()
    {
        InnerWidget<entity_hierarchy>();
        InnerWidget<mesh_renderer>();
        InnerWidget<toon_renderer>();
        InnerWidget<collider>();
    });
}
} // anonymous namespace

namespace nc::sample
{
Benchmarks::Benchmarks(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void Benchmarks::Load(Registry* registry, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(::Widget);
    auto world = registry->GetEcs();
    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    auto ncRandom = modules.Get<Random>();

    world.Emplace<graphics::PointLight>(
        world.Emplace<Entity>({
            .position = Vector3{0.0f, 3.4f, 1.3f},
            .tag = "Point Light"
        }),
        Vector3(0.3f, 0.3f, 0.3f),
        Vector3(0.8f, 0.6f, 1.0f),
        1200.0f
    );

    const auto cameraHandle = world.Emplace<Entity>({.tag = "Main Camera"});
    auto& camera = world.Emplace<graphics::SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    ncGraphics->SetCamera(&camera);

    // Entity Hierarchy
    {
        const auto handle = world.Emplace<Entity>({.tag = "Hierarchy Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            SpawnBehavior{
                .minPosition = Vector3{-40.0f, 0.0f, 20.0f},
                .maxPosition = Vector3{40.0f, 0.0f, 50.0f},
                .minRotation = Vector3::Zero(),
                .maxRotation = Vector3::Zero()
            },
            [world](Entity entity) mutable{
                world.Emplace<graphics::MeshRenderer>(entity);
                world.Emplace<FrameLogic>(entity, [](Entity self, Registry* r, float dt){
                    auto transform = r->Get<Transform>(self);
                    transform->Rotate(Vector3::Up(), 0.3f * dt);
                });

                auto parent = entity;
                auto count = 200;
                while (count-- > 0)
                {
                    const auto child = world.Emplace<Entity>({
                        .position = Vector3::Up(),
                        .rotation = Quaternion::FromAxisAngle(Vector3::Up(), 0.05f),
                        .parent = parent
                    });

                    world.Emplace<graphics::MeshRenderer>(child);
                    parent = child;
                }
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::entity_hierarchy::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::entity_hierarchy::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::entity_hierarchy::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Mesh Renderer
    {
        const auto handle = world.Emplace<Entity>({.tag = "MeshRenderer Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            SpawnBehavior{
                .minPosition = Vector3{-15.0f, -15.0f, 20.0f},
                .maxPosition = Vector3{15.0f, 15.0f, 50.0f},
                .minRotation = Vector3::Zero(),
                .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
            },
            [world](Entity entity) mutable{
                world.Emplace<graphics::MeshRenderer>(entity);
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
            SpawnBehavior{
                .minPosition = Vector3{-15.0f, -15.0f, 20.0f},
                .maxPosition = Vector3{15.0f, 15.0f, 50.0f},
                .minRotation = Vector3::Zero(),
                .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
            },
            [world](Entity entity) mutable{
                world.Emplace<graphics::ToonRenderer>(entity);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::toon_renderer::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::toon_renderer::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::toon_renderer::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }

    // Collider
    {
        const auto handle = world.Emplace<Entity>({.tag = "Collider Spawner"});
        auto& spawner = world.Emplace<Spawner>(
            handle,
            ncRandom,
            SpawnBehavior{
                .minPosition = Vector3{-420.0f, -420.0f, 20.0f},
                .maxPosition = Vector3{420.0f, 420.0f, 420.0f},
                .minRotation = Vector3::Zero(),
                .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
            },
            [registry](Entity entity) mutable{
                registry->Add<graphics::MeshRenderer>(entity, asset::CubeMesh, GreenPbrMaterial);
                registry->Add<physics::Collider>(entity, physics::BoxProperties{}, false);
            }
        );

        world.Emplace<FrameLogic>(handle, InvokeFreeComponent<Spawner>{});
        ::collider::GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, &spawner);
        ::collider::SpawnCallback = std::bind_front(&Spawner::StageSpawn, &spawner);
        ::collider::DestroyCallback = std::bind_front(&Spawner::StageDestroy, &spawner);
    }
}

void Benchmarks::Unload()
{
    m_sampleUI->SetWidgetCallback(nullptr);
}
} // namespace nc::sample
