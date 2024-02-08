#include "CollisionBenchmark.h"
#include "shared/spawner/Spawner.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/physics/Collider.h"

namespace
{
std::function<int()> GetDynamicCountCallback = nullptr;
std::function<void(unsigned)> SpawnDynamicCallback = nullptr;
std::function<void(unsigned)> DestroyDynamicCallback = nullptr;
std::function<int()> GetStaticCountCallback = nullptr;
std::function<void(unsigned)> SpawnStaticCallback = nullptr;
std::function<void(unsigned)> DestroyStaticCallback = nullptr;

int DynamicCount = 100;
int StaticCount = 100;

void Widget()
{
    ImGui::Text("Collision Benchmark");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ImGui::Text("Count Static/Dynamic: %d / %d", GetStaticCountCallback(), GetDynamicCountCallback());
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

        ImGui::Text("Dynamic Cubes");
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("##spawndynamiccount", &DynamicCount);
        DynamicCount = nc::Clamp(DynamicCount, 1, 30000);

        ImGui::SameLine();

        if(ImGui::Button("Spawn##dynamic", {100, 20}))
            SpawnDynamicCallback(DynamicCount);
        
        ImGui::SameLine();

        if(ImGui::Button("Destroy##dynamic", {100, 20}))
            DestroyDynamicCallback(DynamicCount);
        
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

        ImGui::Text("Static Cubes");
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("##spawnstaticcount", &StaticCount);
        StaticCount = nc::Clamp(StaticCount, 1, 30000);

        ImGui::SameLine();

        if(ImGui::Button("Spawn##static", {100, 20}))
            SpawnStaticCallback(StaticCount);

        ImGui::SameLine();

        if(ImGui::Button("Destroy##static", {100, 20}))
            DestroyStaticCallback(StaticCount);
        
    } ImGui::EndChild();
}
} // anonymous namespace

namespace nc::sample
{
CollisionBenchmark::CollisionBenchmark(SampleUI* ui)
    : m_sampleUI{ui}
{
    ui->SetWidgetCallback(::Widget);
}

CollisionBenchmark::~CollisionBenchmark() noexcept
{
    m_sampleUI->SetWidgetCallback(nullptr);
}

void CollisionBenchmark::Load(Registry* registry, ModuleProvider modules)
{
    auto camera = registry->Add<graphics::Camera>(registry->Add<Entity>({.tag = "Main Camera"}));
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Cube Spawner Options
    SpawnBehavior spawnBehavior
    {
        .minPosition = Vector3{-420.0f, -420.0f, 20.0f},
        .maxPosition = Vector3{420.0f, 420.0f, 420.0f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    // Dynamic Cube Spawner
    auto dynamicSpawnExtension = [registry](Entity entity)
    {
        registry->Add<graphics::MeshRenderer>(entity, asset::CubeMesh, prefab::GreenPbrMaterial);
        registry->Add<physics::Collider>(entity, physics::BoxProperties{}, false);
    };
    auto dynamicSpawnerHandle = registry->Add<Entity>({.tag = "DynamicCubeSpawner"});
    auto dynamicSpawner = registry->Add<Spawner>(dynamicSpawnerHandle, modules.Get<Random>(), spawnBehavior, dynamicSpawnExtension);
    registry->Add<FrameLogic>(dynamicSpawnerHandle, InvokeFreeComponent<Spawner>{});

    // Static Cube Spawner
    auto staticSpawnExtension = [registry](Entity entity)
    {
        registry->Add<graphics::MeshRenderer>(entity, asset::CubeMesh, prefab::RedPbrMaterial);
        registry->Add<physics::Collider>(entity, physics::BoxProperties{}, false);
    };
    spawnBehavior.flags = Entity::Flags::Static;
    auto staticSpawnerHandle = registry->Add<Entity>({.tag = "StaticCubeSpawner"});
    auto staticSpawner = registry->Add<Spawner>(staticSpawnerHandle, modules.Get<Random>(), spawnBehavior, staticSpawnExtension);
    registry->Add<FrameLogic>(staticSpawnerHandle, InvokeFreeComponent<Spawner>{});

    // Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(1.0f, 0.7f, 1.0f), Vector3(0.8f, 0.6f, 1.0f), 1200.0f);

    // UI Callbacks
    GetDynamicCountCallback = std::bind(&Spawner::GetObjectCount, dynamicSpawner);
    SpawnDynamicCallback = std::bind(&Spawner::StageSpawn, dynamicSpawner, std::placeholders::_1);
    DestroyDynamicCallback = std::bind(&Spawner::StageDestroy, dynamicSpawner, std::placeholders::_1);
    GetStaticCountCallback = std::bind(&Spawner::GetObjectCount, staticSpawner);
    SpawnStaticCallback = std::bind(&Spawner::StageSpawn, staticSpawner, std::placeholders::_1);
    DestroyStaticCallback = std::bind(&Spawner::StageDestroy, staticSpawner, std::placeholders::_1);
}

void CollisionBenchmark::Unload()
{
    GetDynamicCountCallback = nullptr;
    SpawnDynamicCallback = nullptr;
    DestroyDynamicCallback = nullptr;
    SpawnStaticCallback = nullptr;
    DestroyStaticCallback = nullptr;
}
} // namespace nc::sample

