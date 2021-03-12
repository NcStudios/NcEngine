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
    void CollisionEvents::Load()
    {
        // Setup
        m_sceneHelper.Setup(true, true, Widget);

        // Camera
        auto cameraHandle = CreateEntity({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = AddComponent<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        // Objects
        auto blueCube = prefab::Create(prefab::Resource::CubeBlue, {.tag = "Blue Cube"});
        AddComponent<Collider>(blueCube, ColliderInfo{});
        AddComponent<WasdController>(blueCube, 2.0f);
        AddComponent<CollisionLogger>(blueCube);

        auto smallCube = prefab::Create(prefab::Resource::Cube, {.position = Vector3{2.0f, 0.0f, 2.5f}, .tag = "Small Cube", .isStatic = true});
        AddComponent<Collider>(smallCube, ColliderInfo{});
        AddComponent<CollisionLogger>(smallCube);

        auto bigCube = prefab::Create(prefab::Resource::Cube, {.position = Vector3::Right() * 5.0f, .scale = Vector3::Splat(3.0f), .tag = "Big Cube"});
        AddComponent<Collider>(bigCube, ColliderInfo{});
        AddComponent<CollisionLogger>(bigCube);

        auto longBox = prefab::Create(prefab::Resource::Cube, {.position = Vector3::Back() * 3.0f, .scale = Vector3{5.0f, 1.0f, 1.0f}, .tag = "Long Box"});
        AddComponent<Collider>(longBox, ColliderInfo{});
        AddComponent<CollisionLogger>(longBox);

        auto smallSphere = prefab::Create(prefab::Resource::Sphere, {.position = Vector3{-2.0f, 0.0f, 2.5f}, .tag = "Small Sphere", .isStatic = true});
        AddComponent<Collider>(smallSphere, ColliderInfo{.type = ColliderType::Sphere});
        AddComponent<CollisionLogger>(smallSphere);

        auto bigSphere = prefab::Create(prefab::Resource::Sphere, {.position = Vector3::Left() * 5.0f, .scale = Vector3::Splat(3.0f), .tag = "Big Sphere"});
        AddComponent<Collider>(bigSphere, ColliderInfo{.type = ColliderType::Sphere});
        AddComponent<CollisionLogger>(bigSphere);

        float startPos = -20.0f;
        float spacing = 2.0f;

        for(int i = 0; i < 50; ++i)
        {
            auto cube = prefab::Create(prefab::Resource::Cube, {.position = Vector3{startPos + i * spacing, 0.0f, 5.0f}, .isStatic = true});
            AddComponent<Collider>(cube, ColliderInfo{});
            AddComponent<CollisionLogger>(cube);
        }
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project