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
    void CollisionBenchmark::Load()
    {
        // Setup
        m_sceneHelper.Setup(false, true, Widget);

        auto camera = AddComponent<Camera>(CreateEntity({.tag = "Main Camera"}));
        camera::SetMainCamera(camera);

        // Cube Spawner Options
        SpawnBehavior spawnBehavior
        {
            .positionOffset = Vector3{0.0f, 0.0f, 35.0f},
            .positionRandomRange = Vector3::Splat(420.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f)
        };

        auto spawnExtension = [](EntityHandle handle)
        {
            AddComponent<Collider>(handle, ColliderInfo{});
        };
        
        // Dynamic Cube Spawner
        auto dynamicSpawnerHandle = CreateEntity({.tag = "DynamicCubeSpawner"});
        auto dynamicSpawner = AddComponent<Spawner>(dynamicSpawnerHandle, prefab::Resource::Cube, spawnBehavior, spawnExtension);

        // Static Cube Spawner
        spawnBehavior.spawnAsStaticEntity = true;
        auto staticSpawnerHandle = CreateEntity({.tag = "StaticCubeSpawner"});
        auto staticSpawner = AddComponent<Spawner>(staticSpawnerHandle, prefab::Resource::CubeRed, spawnBehavior, spawnExtension);

        auto fpsTrackerHandle = CreateEntity({.tag = "FpsTracker"});
        auto fpsTracker = AddComponent<FPSTracker>(fpsTrackerHandle);

        // UI Callbacks
        GetDynamicCountCallback = std::bind(Spawner::GetObjectCount, dynamicSpawner);
        SpawnDynamicCallback = std::bind(Spawner::Spawn, dynamicSpawner, std::placeholders::_1);
        DestroyDynamicCallback = std::bind(Spawner::Destroy, dynamicSpawner, std::placeholders::_1);
        GetStaticCountCallback = std::bind(Spawner::GetObjectCount, staticSpawner);
        SpawnStaticCallback = std::bind(Spawner::Spawn, staticSpawner, std::placeholders::_1);
        DestroyStaticCallback = std::bind(Spawner::Destroy, staticSpawner, std::placeholders::_1);
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