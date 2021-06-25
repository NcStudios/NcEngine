#include "SpawnTest.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "KillBox.h"
#include "shared/spawner/FixedIntervalSpawner.h"
#include "shared/FPSTracker.h"
#include "shared/SceneNavigationCamera.h"

namespace
{
    std::function<float()> GetFPSCallback = nullptr;
    std::function<void(float)> SetSpawnRateFunc = nullptr;

    float spawnRate = 0.2f;

    void Widget()
    {
        ImGui::Text("Spawn Test");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("-middle mouse button + drag to pan");
            ImGui::Text("-right mouse button + drag to look");
            ImGui::Text("-mouse wheel to zoom");
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ImGui::Text("FPS: %.1f", GetFPSCallback());
            
            if(ImGui::DragFloat("Spawn Rate", &spawnRate, 0.001f, 0.00001f, 5.0f, "%.5f"))
                SetSpawnRateFunc(spawnRate);
            
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void SpawnTest::Load(registry_type* registry)
    {
        // Setup
        m_sceneHelper.Setup(registry, true, false, Widget);

        // Fps Tracker
        auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
        GetFPSCallback = std::bind(FPSTracker::GetFPS, fpsTracker);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, -9.0f, -100.0f}, .tag = "SceneNavigationCamera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.25f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        // Point Light
        PointLight::Properties lightProperties
        {
            .ambient = Vector3::One(),
            .diffuseIntensity = 4.5f,
            .attConst = 1.0f,
            .attLin = 0.0001f
        };
        auto lvHandle = registry->Add<Entity>({.tag = "Point Light"});
        auto pointLight = registry->Add<PointLight>(lvHandle, PointLight::Properties{});
        pointLight->Set(lightProperties);

        // Collider that destroys anything leaving its bounded area
        auto killBox = registry->Add<Entity>({.scale = Vector3::Splat(100.0f), .tag = "KillBox", .flags = EntityFlags::Static});
        registry->Add<Collider>(killBox, BoxProperties{}, true);
        registry->Add<KillBox>(killBox, registry);

        // Spawner for stationary cubes
        SpawnBehavior staticCubeBehavior
        {
            .positionRandomRange = Vector3::Splat(55.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
            .spawnAsStaticEntity = true
        };

        auto staticCubeExtension = [registry](Entity handle)
        {
            registry->Get<Transform>(handle)->SetScale(Vector3::Splat(6.0f));
            registry->Add<Collider>(handle, BoxProperties{}, false);
        };

        auto staticCubeSpawnerHandle = registry->Add<Entity>({.tag = "Static Cube Spawner"});
        auto staticCubeSpawner = registry->Add<Spawner>(staticCubeSpawnerHandle, registry, prefab::Resource::CubeBlue, staticCubeBehavior, staticCubeExtension);
        staticCubeSpawner->Spawn(5);
        staticCubeSpawner->SetPrefab(prefab::Resource::CubeRed);
        staticCubeSpawner->Spawn(5);

        // Fixed interval spawner for moving cubes
        SpawnBehavior dynamicCubeBehavior
        {
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
            .velocityRandomRange = Vector3::Splat(10.0f),
            .rotationAxisRandomRange = Vector3::One(),
            .thetaRandomRange = 2.0f
        };

        auto dynamicCubeExtension = [registry](Entity handle)
        {
            registry->Add<Collider>(handle, BoxProperties{}, false);
        };
        
        auto dynamicCubeSpawnerHandle = registry->Add<Entity>({.tag = "Dynamic Cube Spawner"});
        auto* intervalSpawner = registry->Add<FixedIntervalSpawner>(dynamicCubeSpawnerHandle, registry, prefab::Resource::CubeGreen, dynamicCubeBehavior, spawnRate, dynamicCubeExtension);
    
        SetSpawnRateFunc = std::bind(FixedIntervalSpawner::SetSpawnRate, intervalSpawner, std::placeholders::_1);
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project