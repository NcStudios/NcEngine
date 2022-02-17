#include "RenderingBenchmark.h"
#include "NcEngine.h"
#include "imgui/imgui.h"
#include "shared/FreeComponents.h"
#include "shared/spawner/Spawner.h"

#include <functional>

namespace
{
    std::function<int()> GetObjectCountCallback = nullptr;
    std::function<float()> GetFPSCallback = nullptr;
    std::function<void(unsigned)> SpawnCallback = nullptr;
    std::function<void(unsigned)> DestroyCallback = nullptr;

    int SpawnCount = 1000;
    int DestroyCount = 100;

    void Widget()
    {
        ImGui::Text("Rendering Benchmark");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Text("Objects: %d", GetObjectCountCallback());
            ImGui::Text("FPS: %.1f", GetFPSCallback());
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::SetNextItemWidth(100);
            ImGui::InputInt("##spawncount", &SpawnCount);
            SpawnCount = nc::math::Clamp(SpawnCount, 1, 30000);
            if(ImGui::Button("Spawn", {100, 20}))
                SpawnCallback(SpawnCount);
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::SetNextItemWidth(100);
            ImGui::InputInt("##destroycount", &DestroyCount);
            DestroyCount = nc::math::Clamp(DestroyCount, 1, 30000);
            if(ImGui::Button("Destroy", {100, 20}))
                DestroyCallback(DestroyCount);
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void RenderingBenchmark::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();

        // Setup
        m_sceneHelper.Setup(engine, false, false, Widget);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.tag = "Main Camera"});
        auto camera = cameraHandle.add<SceneNavigationCamera>(0.25f, 0.005f, 1.4f);
        cameraHandle.add<FrameLogic>(InvokeFreeComponent<SceneNavigationCamera>{});
        engine->MainCamera()->Set(camera);

        // Spawner
        SpawnBehavior spawnBehavior
        {
            .positionOffset = Vector3{0.0f, 0.0f, 35.0f},
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(std::numbers::pi_v<float> / 2.0f)
        };

        auto spawnerHandle = registry->Add<Entity>({.tag = "Spawner"});
        auto spawner = spawnerHandle.add<Spawner>(prefab::Resource::Cube, spawnBehavior);
        spawnerHandle.add<FrameLogic>(InvokeFreeComponent<Spawner>{});
        GetObjectCountCallback = std::bind_front(&Spawner::GetObjectCount, spawner);
        SpawnCallback = std::bind_front(&Spawner::StageSpawn, spawner);
        DestroyCallback = std::bind_front(&Spawner::StageDestroy, spawner);

        auto fpsHandle = registry->Add<Entity>({.tag = "FPSTracker"});
        auto fpsTracker = fpsHandle.add<FPSTracker>();
        fpsHandle.add<FrameLogic>(InvokeFreeComponent<FPSTracker>{});
        GetFPSCallback = std::bind_front(&FPSTracker::GetFPS, fpsTracker);

        // Lights
        auto lightHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light"});
        lightHandle.add<PointLight>(PointLightInfo{.ambient = Vector3(0.3f, 0.3f, 0.3f),
                                                   .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                                   .diffuseIntensity = 1200.0f});
    }

    void RenderingBenchmark::Unload()
    {
        GetObjectCountCallback = nullptr;
        GetFPSCallback = nullptr;
        SpawnCallback = nullptr;
        DestroyCallback = nullptr;
        m_sceneHelper.TearDown();
    }
}