#include "RenderingBenchmark.h"
#include "shared/FreeComponents.h"
#include "shared/spawner/Spawner.h"

#include "NcEngine.h"
#include "graphics/GraphicsModule.h"
#include "imgui/imgui.h"

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
    RenderingBenchmark::RenderingBenchmark(SampleUI* ui)
    {
        ui->SetWidgetCallback(::Widget);
    }

    void RenderingBenchmark::Load(Registry* registry, ModuleProvider modules)
    {
        // Camera
        auto camera = registry->Add<graphics::Camera>(registry->Add<Entity>({.tag = "Main Camera"}));
        modules.Get<graphics::GraphicsModule>()->SetCamera(camera);

        // Spawner
        SpawnBehavior spawnBehavior
        {
            .minPosition = Vector3{-15.0f, -15.0f, 20.0f},
            .maxPosition = Vector3{15.0f, 15.0f, 50.0f},
            .minRotation = Vector3::Zero(),
            .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
        };

        auto spawnerHandle = registry->Add<Entity>({.tag = "Spawner"});
        auto spawner = registry->Add<Spawner>(spawnerHandle, modules.Get<Random>(), prefab::Resource::Cube, spawnBehavior);
        registry->Add<FrameLogic>(spawnerHandle, InvokeFreeComponent<Spawner>{});

        auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FPSTracker"});
        auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
        registry->Add<FrameLogic>(fpsTrackerHandle, InvokeFreeComponent<FPSTracker>{});

        // Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        registry->Add<graphics::PointLight>(lvHandle, graphics::PointLightInfo{.ambient = Vector3(0.3f, 0.3f, 0.3f),
                                                                               .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                                                               .diffuseIntensity = 1200.0f});

        // UI Callbacks
        GetObjectCountCallback = std::bind(&Spawner::GetObjectCount, spawner);
        SpawnCallback = std::bind(&Spawner::StageSpawn, spawner, std::placeholders::_1);
        DestroyCallback = std::bind(&Spawner::StageDestroy, spawner, std::placeholders::_1);
        GetFPSCallback = std::bind(&FPSTracker::GetFPS, fpsTracker);
    }

    void RenderingBenchmark::Unload()
    {
        GetObjectCountCallback = nullptr;
        GetFPSCallback = nullptr;
        SpawnCallback = nullptr;
        DestroyCallback = nullptr;
    }
}