#include "CollisionBenchmark.h"
#include "NcEngine.h"
#include "imgui/imgui.h"
#include "shared/FreeComponents.h"
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
    void CollisionBenchmark::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();

        // Setup
        m_sceneHelper.Setup(engine, false, true, Widget);

        auto cameraHandle = registry->Add<Entity>({.tag = "Main Camera"});
        auto camera = cameraHandle.add<Camera>();
        engine->MainCamera()->Set(camera);

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
            registry->Add<Collider>(entity, BoxProperties{}, false);
        };

        // Dynamic Cube Spawner
        auto dynamicSpawnerHandle = registry->Add<Entity>({.tag = "DynamicCubeSpawner"});
        auto dynamicSpawner = dynamicSpawnerHandle.add<Spawner>(engine->Random(), prefab::Resource::CubeGreen, spawnBehavior, spawnExtension);
        dynamicSpawnerHandle.add<FrameLogic>(InvokeFreeComponent<Spawner>{});
        GetDynamicCountCallback = std::bind_front(&Spawner::GetObjectCount, dynamicSpawner);
        SpawnDynamicCallback = std::bind_front(&Spawner::StageSpawn, dynamicSpawner);
        DestroyDynamicCallback = std::bind_front(&Spawner::StageDestroy, dynamicSpawner);

        // Static Cube Spawner
        spawnBehavior.flags = Entity::Flags::Static;
        auto staticSpawnerHandle = registry->Add<Entity>({.tag = "StaticCubeSpawner"});
        auto staticSpawner = staticSpawnerHandle.add<Spawner>(engine->Random(), prefab::Resource::CubeRed, spawnBehavior, spawnExtension);
        staticSpawnerHandle.add<FrameLogic>(InvokeFreeComponent<Spawner>{});
        GetStaticCountCallback = std::bind_front(&Spawner::GetObjectCount, staticSpawner);
        SpawnStaticCallback = std::bind_front(&Spawner::StageSpawn, staticSpawner);
        DestroyStaticCallback = std::bind_front(&Spawner::StageDestroy, staticSpawner);

        auto fpsHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = fpsHandle.add<FPSTracker>();
        fpsHandle.add<FrameLogic>(InvokeFreeComponent<FPSTracker>{});
        GetFPSCallback = std::bind_front(&FPSTracker::GetFPS, fpsTracker);

        // Lights
        auto light = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        light.add<PointLight>(PointLightInfo{.ambient = Vector3(1.0f, 0.7f, 1.0f),
                                             .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                             .diffuseIntensity = 1200.0f});
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