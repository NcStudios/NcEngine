#include "SpawnTest.h"
#include "NcEngine.h"
#include "ecs/component/PhysicsBody.h"
#include "random/Random.h"
#include "imgui/imgui.h"
#include "shared/FreeComponents.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/Spawner/Spawner.h"

namespace
{
    std::function<float()> GetFPSCallback = nullptr;
    std::function<void(unsigned)> SpawnFunc = nullptr;
    std::function<void(unsigned)> DestroyFunc = nullptr;

    int SpawnCount = 1000;
    int DestroyCount = 1000;

    void Widget()
    {
        ImGui::Text("Spawn Test");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ImGui::Text("FPS: %.1f", GetFPSCallback());

            ImGui::InputInt("##spawncount", &SpawnCount);
            SpawnCount = nc::math::Clamp(SpawnCount, 1, 20000);

            if(ImGui::Button("Spawn", {100, 20}))
                SpawnFunc(SpawnCount);

            ImGui::InputInt("##destroycount", &DestroyCount);
            DestroyCount = nc::math::Clamp(DestroyCount, 1, 20000);

            if(ImGui::Button("Destroy", {100, 20}))
                DestroyFunc(DestroyCount);
            
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("-middle mouse button + drag to pan");
            ImGui::Text("-right mouse button + drag to look");
            ImGui::Text("-mouse wheel to zoom");
        }
        
        ImGui::EndChild();
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
        auto fpsHandle = registry->Add<Entity>({.tag = "FpsTracker"});
        auto fpsTracker = fpsHandle.add<FPSTracker>();
        fpsHandle.add<FrameLogic>(InvokeFreeComponent<FPSTracker>{});
        GetFPSCallback = std::bind_front(&FPSTracker::GetFPS, fpsTracker);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 35.0f, -100.0f}, .rotation = Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f), .tag = "SceneNavigationCamera"});
        auto camera = cameraHandle.add<SceneNavigationCamera>(0.25f, 0.005f, 1.4f);
        cameraHandle.add<FrameLogic>(InvokeFreeComponent<SceneNavigationCamera>{});
        engine->MainCamera()->Set(camera);

        // Lights
        auto lightHandle = registry->Add<Entity>({.position = Vector3{0.0f, 30.0f, 0.0f}, .tag = "Point Light 1"});
        lightHandle.add<PointLight>(PointLightInfo{.ambient = Vector3(0.275f, 0.27f, 0.27f),
                                                   .diffuseColor = Vector3(0.99f, 1.0f, 0.76f),
                                                   .diffuseIntensity = 1000.0f});

        // Collider that destroys anything leaving its bounded area
        auto killBoxHandle = registry->Add<Entity>({.scale = Vector3::Splat(200.0f), .tag = "KillBox", .flags = Entity::Flags::Static});
        killBoxHandle.add<Collider>(BoxProperties{}, true);
        killBoxHandle.add<CollisionLogic>(nullptr, nullptr, nullptr, DestroyOnTriggerExit);

        auto platformHandle = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, 0.0f}, .scale = Vector3{160.0f, 6.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        platformHandle.add<Collider>(BoxProperties{}, false);

        auto wall1Handle = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, 80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        wall1Handle.add<Collider>(BoxProperties{}, false);

        auto wall2Handle = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, -80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        wall2Handle.add<Collider>(BoxProperties{}, false);

        auto wall3Handle = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{80.0f, 0.0f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        wall3Handle.add<Collider>(BoxProperties{}, false);

        auto wall4Handle = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{-80.0f, 0.0f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
        wall4Handle.add<Collider>(BoxProperties{}, false);

        // Fixed interval spawner for moving cubes
        SpawnBehavior dynamicCubeBehavior
        {
            .positionOffset = Vector3{0.0f, 35.0f, 0.0f},
            .positionRandomRange = Vector3{70.0f, 15.0f, 70.0f},
            .rotationRandomRange = Vector3::Splat(std::numbers::pi_v<float> / 2.0f),
        };

        auto dynamicCubeExtension = [registry](Entity handle)
        {
            registry->Add<Collider>(handle, BoxProperties{}, false);
            registry->Add<PhysicsBody>(handle, PhysicsProperties{.mass = 5.0f});
        };

        auto spawnerHandle = registry->Add<Entity>({});
        auto spawner = spawnerHandle.add<Spawner>(prefab::Resource::CubeTextured, dynamicCubeBehavior, dynamicCubeExtension);
        spawnerHandle.add<FrameLogic>(InvokeFreeComponent<Spawner>{});
        SpawnFunc = std::bind_front(&Spawner::StageSpawn, spawner);
        DestroyFunc = std::bind_front(&Spawner::StageDestroy, spawner);
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project