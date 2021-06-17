#include "CollisionEvents.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/SceneNavigationCamera.h"
#include "WasdController.h"
#include "CollisionLogger.h"

namespace
{
    void Widget()
    {
        ImGui::Text("Collision Events");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Text("Check the log for collision events.");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("-move camera by edge panning");
            ImGui::Text("-zoom with mouse wheel");
            ImGui::Text("-WASD to control the blue cube");
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void CollisionEvents::Load(registry_type* registry)
    {
        // Setup
        m_sceneHelper.Setup(registry, true, false, Widget);

        // Light
        auto lightProperties = PointLight::Properties
        {
            .pos = Vector3::Zero(),
            .ambient = Vector3{0.443f, 0.412f, 0.412f},
            .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
            .diffuseIntensity = 3.0,
            .attConst = 0.0f,
            .attLin = 0.05f,
            .attQuad = 0.0f
        };

        auto light1 = registry->Add<Entity>({.position = Vector3{2.0f, 8.0f, 0.0f}, .tag = "Light 1"});
        registry->Add<PointLight>(light1, lightProperties);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        // Movable Object
        auto blueCube = prefab::Create(registry, prefab::Resource::SphereBlue, {.position = Vector3{0.0f, 0.0f, 0.0f}, .tag = "Blue Cube"});
        registry->Add<Collider>(blueCube, ColliderInfo{.type = ColliderType::Sphere});
        registry->Add<WasdController>(blueCube, registry, 2.0f);
        registry->Add<CollisionLogger>(blueCube, registry);

        // Dynamic Objects
        auto smallGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{3.5f, 0.0f, -1.0f}, .rotation = Quaternion::FromEulerAngles(0.0f, 0.785f, 0.0f), .tag = "Small Green Cube"});
        registry->Add<Collider>(smallGreenCube, ColliderInfo{});
        registry->Add<WasdController2>(smallGreenCube, registry, 2.0f);
        registry->Add<CollisionLogger>(smallGreenCube, registry);

        auto smallGreenSphere = prefab::Create(registry, prefab::Resource::SphereGreen, {.position = Vector3{-3.5f, 0.0f, -1.0f}, .tag = "Small Green Sphere"});
        registry->Add<Collider>(smallGreenSphere, ColliderInfo{.type = ColliderType::Sphere});
        registry->Add<CollisionLogger>(smallGreenSphere, registry);

        auto bigGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, 6.5f}, .scale = Vector3::Splat(2.0f), .tag = "Big Green Cube"});
        registry->Add<Collider>(bigGreenCube, ColliderInfo{});
        registry->Add<CollisionLogger>(bigGreenCube, registry);


        // Static Objects
        auto bigRedCube = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{3.0f, 0.0f, 3.0f}, .scale = Vector3::Splat(1.0f), .tag = "Big Red Cube", .flags = EntityFlags::Static});
        registry->Add<Collider>(bigRedCube, ColliderInfo{});
        registry->Add<CollisionLogger>(bigRedCube, registry);

        auto bigRedSphere = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{-4.0f, 0.0f, 3.0f}, .scale = Vector3::Splat(3.0f), .tag = "Big Red Sphere", .flags = EntityFlags::Static});
        registry->Add<Collider>(bigRedSphere, ColliderInfo{.type = ColliderType::Sphere});
        registry->Add<CollisionLogger>(bigRedSphere, registry);

        auto longRedBox = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3::Back() * 3.0f, .scale = Vector3{5.0f, 1.0f, 1.0f}, .tag = "Long Red Box", .flags = EntityFlags::Static});
        registry->Add<Collider>(longRedBox, ColliderInfo{});
        registry->Add<CollisionLogger>(longRedBox, registry);
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project