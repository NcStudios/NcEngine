#include "CollisionEvents.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/EdgePanCamera.h"
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
        auto cameraHandle = CreateEntity({.position = Vector3{3.6f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = AddComponent<EdgePanCamera>(cameraHandle);
        camera::SetMainCamera(camera);

        // Cubes
        auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(180.0f), 0.0f, 0.0f);
        auto cube1 = prefab::Create(prefab::Resource::CubeBlue, {.position = Vector3{2.0f, 0.0f, 0.4f}, .rotation = rot, .tag = "Blue Cube"});
        AddComponent<Collider>(cube1, ColliderType::Box, Vector3::One());
        AddComponent<WasdController>(cube1, 2.0f);
        AddComponent<CollisionLogger>(cube1);

        auto cube2 = prefab::Create(prefab::Resource::CubeRed, {.position = Vector3{6.0f, 0.0f, 0.4f}, .rotation = rot, .tag = "Red Cube"});
        AddComponent<Collider>(cube2, ColliderType::Box, Vector3::One());
        AddComponent<CollisionLogger>(cube2);

        auto sphere = prefab::Create(prefab::Resource::Sphere, {.position = Vector3{4.0f, 0.0f, 4.0f}, .tag = "Sphere"});
        AddComponent<Collider>(sphere, ColliderType::Sphere, Vector3::One());
        AddComponent<CollisionLogger>(sphere);
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project