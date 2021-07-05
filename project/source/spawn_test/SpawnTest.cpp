#include "SpawnTest.h"
#include "MainCamera.h"
#include "random/Random.h"
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
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, -15.0f, -20.0f}, .rotation = Quaternion::FromEulerAngles(0.25f, 0.0f, 0.0f), .tag = "SceneNavigationCamera"});
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

        // // Spawner for stationary cubes
        // SpawnBehavior staticCubeBehavior
        // {
        //     .positionRandomRange = Vector3::Splat(55.0f),
        //     .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
        //     .spawnAsStaticEntity = true
        // };

        // auto staticCubeExtension = [registry](Entity handle)
        // {
        //     registry->Get<Transform>(handle)->SetScale(Vector3::Splat(6.0f));
        //     registry->Add<Collider>(handle, BoxProperties{}, false);
        // };

        // auto staticCubeSpawnerHandle = registry->Add<Entity>({.tag = "Static Cube Spawner"});
        // auto staticCubeSpawner = registry->Add<Spawner>(staticCubeSpawnerHandle, registry, prefab::Resource::CubeBlue, staticCubeBehavior, staticCubeExtension);
        // staticCubeSpawner->Spawn(5);
        // staticCubeSpawner->SetPrefab(prefab::Resource::CubeRed);
        // staticCubeSpawner->Spawn(5);

        auto rotation = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
        //auto rotation2 = Quaternion::FromEulerAngles(math::DegreesToRadians(60.0f), 0.0f, 0.0f);

        // auto platform = prefab::Create(registry, prefab::Resource::DiscRed, {.position = Vector3{0.0f, -25.0f, 0.0f}, .rotation = rotation, .scale = Vector3::Splat(500.0f), .tag = "Platform", .flags = EntityFlags::Static});
        // registry->Add<Collider>(platform, HullProperties{.assetPath = "project/assets/mesh_colliders/coin.nca"}, false);
        // registry->Add<PhysicsBody>(platform, PhysicsProperties{.mass = 10000.0f, .useGravity = false});

        auto platform = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{0.0f, -25.0f, 0.0f}, .scale = Vector3{35.0f, 6.0f, 35.0f}, .tag = "Platform", .flags = EntityFlags::Static});
        registry->Add<Collider>(platform, BoxProperties{}, false);
        registry->Add<PhysicsBody>(platform, PhysicsProperties{.mass = 0.0f, .useGravity = false});

        //auto platform = prefab::Create(registry, prefab::Resource::SphereBlue, {.position = Vector3{0.0f, -75.0f, 0.0f}, .scale = Vector3{75.0f, 75.0f, 75.0f}, .tag = "Platform", .flags = EntityFlags::Static});
        //registry->Add<Collider>(platform, SphereProperties{}, false);
        //registry->Add<PhysicsBody>(platform, PhysicsProperties{.mass = 1000.0f, .useGravity = false});

        // for(size_t i = 0u; i < 25u; ++i)
        // {
        //     float xPos = -12.5f + i * 1.0f;

        //     for(size_t j = 0u; j < 25u; ++j)
        //     {
        //         float zPos = -12.5f + j * 1.0f;

        //         auto chunk = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{xPos, -25.0f, zPos}, .scale = Vector3{1.0f, 3.0f, 1.0f}, .flags = EntityFlags::Static});
        //         registry->Add<Collider>(chunk, BoxProperties{}, false);
        //         registry->Add<PhysicsBody>(chunk, PhysicsProperties{.useGravity = false});
        //     }
        // }


        //auto bottomDisc = prefab::Create(registry, prefab::Resource::DiscGreen, {.position = Vector3{0.0f, -15.0f, 0.0f}, .rotation = rotation, .scale = Vector3::Splat(5.0f), .flags = EntityFlags::None});
        //registry->Add<Collider>(bottomDisc, HullProperties{.assetPath = "project/assets/mesh_colliders/coin.nca"}, false);
        //registry->Add<PhysicsBody>(bottomDisc, PhysicsProperties{.mass = 1.0f, .useGravity = false});

        //auto topDisc = prefab::Create(registry, prefab::Resource::DiscGreen, {.position = Vector3{0.5f, 15.0f, 0.0f}, .rotation = rotation, .scale = Vector3::Splat(10.0f)});
        //registry->Add<Collider>(topDisc, HullProperties{.assetPath = "project/assets/mesh_colliders/coin.nca"}, false);
        //registry->Add<PhysicsBody>(topDisc, PhysicsProperties{.mass = 0.1f});


        auto bottomBox = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, 0.0f}, .scale = Vector3::Splat(2.0f), .flags = EntityFlags::None});
        registry->Add<Collider>(bottomBox, BoxProperties{}, false);
        registry->Add<PhysicsBody>(bottomBox, PhysicsProperties{.mass = 2.0f, .useGravity = true});

        auto topBox = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{1.0f, 7.0f, 1.0f}, .scale = Vector3::Splat(2.0f)});
        registry->Add<Collider>(topBox, BoxProperties{}, false);
        registry->Add<PhysicsBody>(topBox, PhysicsProperties{.angularFreedom = Vector3::Up(), .mass = 2.0f});

        //auto topSphere = prefab::Create(registry, prefab::Resource::SphereGreen, {.position = Vector3{0.0f, -2.0f, 0.0f}, .scale = Vector3::Splat(3.0f)});
        //registry->Add<Collider>(topSphere, SphereProperties{}, false);
        //registry->Add<PhysicsBody>(topSphere, PhysicsProperties{});

        // auto table = prefab::Create(registry, prefab::Resource::Table, {.position = Vector3{0.0f, -100.0f, 0.0f}});

        // // Fixed interval spawner for moving cubes
        // SpawnBehavior dynamicCubeBehavior
        // {
        //     .positionRandomRange = Vector3::Splat(15.0f),
        //     .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
        //     //.velocityRandomRange = Vector3::Splat(10.0f),
        //     //.rotationAxisRandomRange = Vector3::One(),
        //     //.thetaRandomRange = 2.0f
        // };

        // auto dynamicCubeExtension = [registry](Entity handle)
        // {
        //     //registry->Add<Collider>(handle, HullProperties{.assetPath = "project/assets/mesh_colliders/coin.nca"}, false);
        //     registry->Add<Collider>(handle, BoxProperties{}, false);
        //     //registry->Add<PhysicsBody>(handle, PhysicsProperties{random::Vec3(Vector3::Zero(), Vector3::Splat(10.0f))});
        //     registry->Add<PhysicsBody>(handle, PhysicsProperties{.mass = 2.0f});
        
        // };
        
        // auto dynamicCubeSpawnerHandle = registry->Add<Entity>({.tag = "Dynamic Cube Spawner"});
        // auto* intervalSpawner = registry->Add<FixedIntervalSpawner>(dynamicCubeSpawnerHandle, registry, prefab::Resource::CubeGreen, dynamicCubeBehavior, spawnRate, dynamicCubeExtension);
    
        // SetSpawnRateFunc = std::bind(FixedIntervalSpawner::SetSpawnRate, intervalSpawner, std::placeholders::_1);
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project