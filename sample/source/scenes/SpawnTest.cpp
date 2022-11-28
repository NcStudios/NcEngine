#include "SpawnTest.h"
#include "shared/FreeComponents.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/Spawner/Spawner.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/math/Random.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

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
        SpawnCount = nc::Clamp(SpawnCount, 1, 20000);

        if(ImGui::Button("Spawn", {100, 20}))
            SpawnFunc(SpawnCount);

        ImGui::InputInt("##destroycount", &DestroyCount);
        DestroyCount = nc::Clamp(DestroyCount, 1, 20000);

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
SpawnTest::SpawnTest(SampleUI* ui)
{
    ui->SetWidgetCallback(::Widget);
}

void SpawnTest::Load(Registry* registry, ModuleProvider modules)
{
    // Fps Tracker
    auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
    auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
    registry->Add<FrameLogic>(fpsTrackerHandle, InvokeFreeComponent<FPSTracker>{});
    GetFPSCallback = std::bind(&FPSTracker::GetFPS, fpsTracker);

    // Camera
    auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 35.0f, -100.0f}, .rotation = Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f), .tag = "SceneNavigationCamera"});
    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 30.0f, 0.0f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, graphics::PointLightInfo{.ambient = Vector3(0.275f, 0.27f, 0.27f),
                                                                            .diffuseColor = Vector3(0.99f, 1.0f, 0.76f),
                                                                            .diffuseIntensity = 1000.0f});

    // Collider that destroys anything leaving its bounded area
    auto killBox = registry->Add<Entity>({.scale = Vector3::Splat(200.0f), .tag = "KillBox", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(killBox, physics::BoxProperties{}, true);
    registry->Add<CollisionLogic>(killBox, nullptr, nullptr, nullptr, DestroyOnTriggerExit);

    auto platform = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, 0.0f}, .scale = Vector3{160.0f, 6.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(platform, physics::BoxProperties{}, false);

    auto w1 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, 80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w1, physics::BoxProperties{}, false);

    auto w2 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, 0.0f, -80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w2, physics::BoxProperties{}, false);

    auto w3 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{80.0f, 0.0f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w3, physics::BoxProperties{}, false);

    auto w4 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{-80.0f, 0.0f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w4, physics::BoxProperties{}, false);

    // Fixed interval spawner for moving cubes
    SpawnBehavior dynamicCubeBehavior
    {
        .minPosition = Vector3{-70.0f, 20.0f, -70.0f},
        .maxPosition = Vector3{70.0f, 50.0f, 70.0f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    auto dynamicCubeExtension = [registry](Entity handle)
    {
        registry->Add<physics::Collider>(handle, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(handle, physics::PhysicsProperties{.mass = 5.0f});
    };

    auto spawner = registry->Add<Entity>({});
    auto spawnerPtr = registry->Add<Spawner>(spawner, modules.Get<Random>(), prefab::Resource::CubeTextured, dynamicCubeBehavior, dynamicCubeExtension);
    registry->Add<FrameLogic>(spawner, InvokeFreeComponent<Spawner>{});
    SpawnFunc = std::bind(&Spawner::StageSpawn, spawnerPtr, std::placeholders::_1);
    DestroyFunc = std::bind(&Spawner::StageDestroy, spawnerPtr, std::placeholders::_1);
}
}
