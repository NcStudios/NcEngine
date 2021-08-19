#include "CollisionBenchmark.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "shared/FPSTracker.h"
#include "shared/spawner/Spawner.h"

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
    void CollisionBenchmark::Load(registry_type* registry)
    {
        // Setup
        m_sceneHelper.Setup(registry, false, true, Widget);

        auto camera = registry->Add<Camera>(registry->Add<Entity>({.tag = "Main Camera"}));
        camera::SetMainCamera(camera);

        // Cube Spawner Options
        SpawnBehavior spawnBehavior
        {
            .positionOffset = Vector3{0.0f, 0.0f, 35.0f},
            .positionRandomRange = Vector3::Splat(420.0f),
            .rotationRandomRange = Vector3::Splat(std::numbers::pi / 2.0f)
        };

        auto spawnExtension = [registry](Entity entity)
        {
            registry->Add<Collider>(entity, BoxProperties{}, false);
        };
        
        // Dynamic Cube Spawner
        auto dynamicSpawnerHandle = registry->Add<Entity>({.tag = "DynamicCubeSpawner"});
        auto dynamicSpawner = registry->Add<Spawner>(dynamicSpawnerHandle, registry, prefab::Resource::CubeGreen, spawnBehavior, spawnExtension);

        // Static Cube Spawner
        spawnBehavior.spawnAsStaticEntity = true;
        auto staticSpawnerHandle = registry->Add<Entity>({.tag = "StaticCubeSpawner"});
        auto staticSpawner = registry->Add<Spawner>(staticSpawnerHandle, registry, prefab::Resource::CubeRed, spawnBehavior, spawnExtension);

        auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);

        // UI Callbacks
        GetDynamicCountCallback = std::bind(Spawner::GetObjectCount, dynamicSpawner);
        SpawnDynamicCallback = std::bind(Spawner::StageSpawn, dynamicSpawner, std::placeholders::_1);
        DestroyDynamicCallback = std::bind(Spawner::StageDestroy, dynamicSpawner, std::placeholders::_1);
        GetStaticCountCallback = std::bind(Spawner::GetObjectCount, staticSpawner);
        SpawnStaticCallback = std::bind(Spawner::StageSpawn, staticSpawner, std::placeholders::_1);
        DestroyStaticCallback = std::bind(Spawner::StageDestroy, staticSpawner, std::placeholders::_1);
        GetFPSCallback = std::bind(FPSTracker::GetFPS, fpsTracker);
    }

    void CollisionBenchmark::Unload()
    {
        GetDynamicCountCallback = nullptr;
        SpawnDynamicCallback = nullptr;
        DestroyDynamicCallback = nullptr;
        SpawnStaticCallback = nullptr;
        DestroyStaticCallback = nullptr;
        GetFPSCallback = nullptr;
        m_sceneHelper.TearDown();
    }
}