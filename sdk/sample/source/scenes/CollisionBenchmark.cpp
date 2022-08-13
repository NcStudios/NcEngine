#include "CollisionBenchmark.h"
#include "shared/FreeComponents.h"
#include "shared/spawner/Spawner.h"

#include "NcEngine.h"
#include "graphics/GraphicsModule.h"
#include "imgui/imgui.h"

namespace
{
    std::function<int()> GetDynamicCountCallback = nullptr;
    std::function<void(unsigned)> SpawnDynamicCallback = nullptr;
    std::function<void(unsigned)> DestroyDynamicCallback = nullptr;
    std::function<int()> GetStaticCountCallback = nullptr;
    std::function<void(unsigned)> SpawnStaticCallback = nullptr;
    std::function<void(unsigned)> DestroyStaticCallback = nullptr;
    std::function<float()> GetFPSCallback = nullptr;

    int DynamicCount = 100;
    int StaticCount = 100;

    void Widget()
    {
        ImGui::Text("Collision Benchmark");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Text("Count Static/Dynamic: %d / %d", GetStaticCountCallback(), GetDynamicCountCallback());
            ImGui::Text("FPS: %.1f", GetFPSCallback());
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::Text("Dynamic Cubes");
            ImGui::SetNextItemWidth(100);
            ImGui::InputInt("##spawndynamiccount", &DynamicCount);
            DynamicCount = nc::math::Clamp(DynamicCount, 1, 30000);

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
            StaticCount = nc::math::Clamp(StaticCount, 1, 30000);

            ImGui::SameLine();

            if(ImGui::Button("Spawn##static", {100, 20}))
                SpawnStaticCallback(StaticCount);

            ImGui::SameLine();

            if(ImGui::Button("Destroy##static", {100, 20}))
                DestroyStaticCallback(StaticCount);
            
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    CollisionBenchmark::CollisionBenchmark(SampleUI* ui)
    {
        ui->SetWidgetCallback(::Widget);
    }

    void CollisionBenchmark::Load(Registry* registry, ModuleProvider modules)
    {
        auto camera = registry->Add<Camera>(registry->Add<Entity>({.tag = "Main Camera"}));
        modules.Get<GraphicsModule>()->SetCamera(camera);

        // Cube Spawner Options
        SpawnBehavior spawnBehavior
        {
            .minPosition = Vector3{-420.0f, -420.0f, 20.0f},
            .maxPosition = Vector3{420.0f, 420.0f, 420.0f},
            .minRotation = Vector3::Zero(),
            .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
        };

        auto spawnExtension = [registry](Entity entity)
        {
            registry->Add<physics::Collider>(entity, physics::BoxProperties{}, false);
        };

        // Dynamic Cube Spawner
        auto dynamicSpawnerHandle = registry->Add<Entity>({.tag = "DynamicCubeSpawner"});
        auto dynamicSpawner = registry->Add<Spawner>(dynamicSpawnerHandle, modules.Get<Random>(), prefab::Resource::CubeGreen, spawnBehavior, spawnExtension);
        registry->Add<FrameLogic>(dynamicSpawnerHandle, InvokeFreeComponent<Spawner>{});

        // Static Cube Spawner
        spawnBehavior.flags = Entity::Flags::Static;
        auto staticSpawnerHandle = registry->Add<Entity>({.tag = "StaticCubeSpawner"});
        auto staticSpawner = registry->Add<Spawner>(staticSpawnerHandle, modules.Get<Random>(), prefab::Resource::CubeRed, spawnBehavior, spawnExtension);
        registry->Add<FrameLogic>(staticSpawnerHandle, InvokeFreeComponent<Spawner>{});

        auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
        registry->Add<FrameLogic>(fpsTrackerHandle, InvokeFreeComponent<FPSTracker>{});

        // Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(1.0f, 0.7f, 1.0f),
                                                           .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                                           .diffuseIntensity = 1200.0f});

        // UI Callbacks
        GetDynamicCountCallback = std::bind(&Spawner::GetObjectCount, dynamicSpawner);
        SpawnDynamicCallback = std::bind(&Spawner::StageSpawn, dynamicSpawner, std::placeholders::_1);
        DestroyDynamicCallback = std::bind(&Spawner::StageDestroy, dynamicSpawner, std::placeholders::_1);
        GetStaticCountCallback = std::bind(&Spawner::GetObjectCount, staticSpawner);
        SpawnStaticCallback = std::bind(&Spawner::StageSpawn, staticSpawner, std::placeholders::_1);
        DestroyStaticCallback = std::bind(&Spawner::StageDestroy, staticSpawner, std::placeholders::_1);
        GetFPSCallback = std::bind(&FPSTracker::GetFPS, fpsTracker);
    }

    void CollisionBenchmark::Unload()
    {
        GetDynamicCountCallback = nullptr;
        SpawnDynamicCallback = nullptr;
        DestroyDynamicCallback = nullptr;
        SpawnStaticCallback = nullptr;
        DestroyStaticCallback = nullptr;
        GetFPSCallback = nullptr;
    }
} // namespace nc::sample
