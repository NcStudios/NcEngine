#include "SpawnTest.h"
#include "NcEngine.h"
#include "ecs/component/PhysicsBody.h"
#include "random/Random.h"
#include "imgui/imgui.h"
#include "KillBox.h"
#include "shared/spawner/FixedIntervalSpawner.h"
#include "shared/FPSTracker.h"
#include "shared/SceneNavigationCamera.h"
#include "collision_events/WasdController.h"

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
    void SpawnTest::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();
        
        // Setup
        m_sceneHelper.Setup(engine, true, false, Widget);

        // Fps Tracker
        auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
        GetFPSCallback = std::bind(FPSTracker::GetFPS, fpsTracker);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, -15.0f, -20.0f}, .rotation = Quaternion::FromEulerAngles(0.25f, 0.0f, 0.0f), .tag = "SceneNavigationCamera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.25f, 0.005f, 1.4f);
        engine->MainCamera()->Set(camera);

        // Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, -18.0f, 0.0f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(0.275f, 0.27f, 0.27f),
                                                           .diffuseColor = Vector3(0.99f, 1.0f, 0.76f),
                                                           .diffuseIntensity = 5.0f});

        // Collider that destroys anything leaving its bounded area
        auto killBox = registry->Add<Entity>({.scale = Vector3::Splat(100.0f), .tag = "KillBox", .flags = Entity::Flags::Static});
        registry->Add<Collider>(killBox, BoxProperties{}, true);
        registry->Add<KillBox>(killBox, registry);

        // Ground
        auto platform = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, -25.0f, 0.0f}, .scale = Vector3{35.0f, 6.0f, 35.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        registry->Add<Collider>(platform, BoxProperties{}, false);

        // Moveable box
        auto bottomBox = prefab::Create(registry, prefab::Resource::CubeTextured, {.position = Vector3{0.0f, 0.0f, 0.0f}, .scale = Vector3::Splat(2.0f), .flags = Entity::Flags::None});
        registry->Add<Collider>(bottomBox, BoxProperties{}, false);
        registry->Add<PhysicsBody>(bottomBox, PhysicsProperties{.mass = 4.0f, .useGravity = true});
        registry->Add<WasdController>(bottomBox, registry, 4.0f);

        // Fixed interval spawner for moving cubes
        SpawnBehavior dynamicCubeBehavior
        {
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(std::numbers::pi / 2.0f),
        };

        auto dynamicCubeExtension = [registry](Entity handle)
        {
            registry->Add<Collider>(handle, BoxProperties{}, false);
            registry->Add<PhysicsBody>(handle, PhysicsProperties{.mass = 5.0f});
        };
        
        auto dynamicCubeSpawnerHandle = registry->Add<Entity>({.tag = "Dynamic Cube Spawner"});
        auto* intervalSpawner = registry->Add<FixedIntervalSpawner>(dynamicCubeSpawnerHandle, registry, prefab::Resource::CubeTextured, dynamicCubeBehavior, spawnRate, dynamicCubeExtension);
    
        SetSpawnRateFunc = std::bind(FixedIntervalSpawner::SetSpawnRate, intervalSpawner, std::placeholders::_1);
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project