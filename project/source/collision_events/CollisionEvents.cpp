#include "CollisionEvents.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
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
            ImGui::Text("Controls:");
            ImGui::Spacing();
            ImGui::Text("-move camera by edge panning");
            ImGui::Text("-zoom with mouse wheel");
            ImGui::Text("-rotate with x and y keys");
            ImGui::Text("-WASD to control box");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("Check the log for collision events.");
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void CollisionEvents::Load()
    {
        m_sceneHelper.Setup(true, true, Widget);

        //Camera
        auto camTransform = camera::GetMainCameraTransform();
        camTransform->Set(Vector3{3.6f, 6.1f, -6.5f}, Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), Vector3::One());
        auto camHandle = camTransform->GetParentHandle();
        AddComponent<EdgePanCamera>(camHandle);

        // Objects
        auto colliderScale = Vector3::One();
        auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(180.0f), 0.0f, 0.0f);
        auto cube1 = prefab::Create(prefab::Resource::Cube, Vector3{2.0f, 0.0f, 0.4f}, rot, Vector3::One(), "Cube1");
        AddComponent<Collider>(cube1, colliderScale);
        AddComponent<WasdController>(cube1, 2.0f);
        AddComponent<CollisionLogger>(cube1);

        auto cube2 = prefab::Create(prefab::Resource::Cube, Vector3{6.0f, 0.0f, 0.4f}, Quaternion::Identity(), Vector3::One(), "Cube2");
        AddComponent<Collider>(cube2, colliderScale);
        AddComponent<CollisionLogger>(cube2);
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project