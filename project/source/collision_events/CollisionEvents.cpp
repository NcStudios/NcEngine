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
        m_sceneHelper.Setup(registry, true, true, Widget);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        // Objects
        auto blueCube = prefab::Create(registry, prefab::Resource::CubeBlue, {.tag = "Blue Cube"});
        registry->Add<Collider>(blueCube, ColliderInfo{});
        registry->Add<WasdController>(blueCube, registry, 2.0f);
        registry->Add<CollisionLogger>(blueCube, registry);

        auto smallCube = prefab::Create(registry, prefab::Resource::Cube, {.position = Vector3{2.0f, 0.0f, 2.5f}, .tag = "Small Cube", .flags = EntityFlags::Static});
        registry->Add<Collider>(smallCube, ColliderInfo{});
        registry->Add<CollisionLogger>(smallCube, registry);

        auto bigCube = prefab::Create(registry, prefab::Resource::Cube, {.position = Vector3::Right() * 5.0f, .scale = Vector3::Splat(3.0f), .tag = "Big Cube"});
        registry->Add<Collider>(bigCube, ColliderInfo{});
        registry->Add<CollisionLogger>(bigCube, registry);

        auto longBox = prefab::Create(registry, prefab::Resource::Cube, {.position = Vector3::Back() * 3.0f, .scale = Vector3{5.0f, 1.0f, 1.0f}, .tag = "Long Box"});
        registry->Add<Collider>(longBox, ColliderInfo{});
        registry->Add<CollisionLogger>(longBox, registry);

        auto smallSphere = prefab::Create(registry, prefab::Resource::Sphere, {.position = Vector3{-2.0f, 0.0f, 2.5f}, .tag = "Small Sphere", .flags = EntityFlags::Static});
        registry->Add<Collider>(smallSphere, ColliderInfo{.type = ColliderType::Sphere});
        registry->Add<CollisionLogger>(smallSphere, registry);

        auto bigSphere = prefab::Create(registry, prefab::Resource::Sphere, {.position = Vector3::Left() * 5.0f, .scale = Vector3::Splat(3.0f), .tag = "Big Sphere"});
        registry->Add<Collider>(bigSphere, ColliderInfo{.type = ColliderType::Sphere});
        registry->Add<CollisionLogger>(bigSphere, registry);

        float startPos = -20.0f;
        float spacing = 2.0f;

        for(int i = 0; i < 50; ++i)
        {
            auto cube = prefab::Create(registry, prefab::Resource::Cube, {.position = Vector3{startPos + i * spacing, 0.0f, 5.0f}, .flags = EntityFlags::Static});
            registry->Add<Collider>(cube, ColliderInfo{});
            registry->Add<CollisionLogger>(cube, registry);
        }
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project