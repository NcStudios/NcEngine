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
        m_sceneHelper.Setup(true, true, Widget);

        //Camera
        auto cameraHandle = CreateEntity(Vector3{3.6f, 6.1f, -6.5f}, Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), Vector3::One(), "Main Camera");
        auto camera = AddComponent<EdgePanCamera>(cameraHandle);
        camera::SetMainCamera(camera);

        // Objects
        auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(180.0f), 0.0f, 0.0f);
        auto cube1 = prefab::Create(prefab::Resource::CubeBlue, Vector3{2.0f, 0.0f, 0.4f}, rot, Vector3::One());
        AddComponent<Collider>(cube1, Vector3::One());
        AddComponent<WasdController>(cube1, 2.0f);
        AddComponent<CollisionLogger>(cube1);

        auto cube2 = prefab::Create(prefab::Resource::CubeRed, Vector3{6.0f, 0.0f, 0.4f}, rot, Vector3::One());
        AddComponent<Collider>(cube2, Vector3::One());
        AddComponent<CollisionLogger>(cube2);
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project