#include "ClickEvents.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/EdgePanCamera.h"
#include "Clickable.h"
#include "ClickHandler.h"

#include <functional>

namespace
{
    std::function<void(nc::physics::LayerMask)> LayerSelectCallback = nullptr;
    const auto MaskNone = nc::physics::LayerMask32None;
    const auto MaskAll = nc::physics::LayerMask32All;
    const auto MaskCoin = nc::physics::LayerMask32{1u};
    const auto MaskToken = nc::physics::LayerMask32{2u};

    void Widget()
    {
        ImGui::Text("Click Events");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            static int maskSelection = MaskAll;
            ImGui::Text("Active Layer");
            ImGui::Spacing();
            if(ImGui::RadioButton("All", &maskSelection, MaskAll) && LayerSelectCallback)
                LayerSelectCallback(MaskAll);
            ImGui::SameLine();
            if(ImGui::RadioButton("None", &maskSelection, MaskNone) && LayerSelectCallback)
                LayerSelectCallback(MaskNone);
            ImGui::SameLine();
            if(ImGui::RadioButton("Coin", &maskSelection, MaskCoin) && LayerSelectCallback)
                LayerSelectCallback(MaskCoin);
            ImGui::SameLine();
            if(ImGui::RadioButton("Token", &maskSelection, MaskToken) && LayerSelectCallback)
                LayerSelectCallback(MaskToken);
            ImGui::Spacing();
            ImGui::Text("Check the log for on click events.");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("Controls:");
            ImGui::Spacing();
            ImGui::Text("-move camera by edge panning");
            ImGui::Text("-zoom with mouse wheel");
            ImGui::Text("-rotate with x and y keys");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void ClickEvents::Load()
    {
        m_sceneHelper.Setup(true, false, Widget);

        // Lights
        auto lvHandle = CreateEntity(Vector3{-2.4f, 12.1f, 0.0f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle);
        auto lvHandle2 = CreateEntity(Vector3{12.1f, 14.5f, 7.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle2);
        auto lvHandle3 = CreateEntity(Vector3{4.1f, 14.5f, 3.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle3);

        //Camera
        auto camTransform = camera::GetMainCameraTransform();
        camTransform->Set(Vector3{3.6f, 6.1f, -6.5f}, Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), Vector3::One());
        auto camHandle = camTransform->GetParentHandle();
        AddComponent<EdgePanCamera>(camHandle);
        auto clickHandler = AddComponent<ClickHandler>(camHandle, MaskAll);
        LayerSelectCallback = std::bind(clickHandler->SetLayer, clickHandler, std::placeholders::_1);

        // Objects
        prefab::Create(prefab::Resource::Table, Vector3{4.0f, -0.4f, 3.0f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), Vector3::Splat(7.5f), "Table");
        
        /** @todo add filtering - layer controlled in widget */
        auto coinHandle = prefab::Create(prefab::Resource::Coin, Vector3{1.6f, 0.0f, 1.6f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::Splat(2.0f), "Coin");
        AddComponent<Clickable>(coinHandle, MaskCoin);
        auto tokenHandle = prefab::Create(prefab::Resource::Token, Vector3{2.0f, 0.0f, 0.4f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::Splat(2.0f), "Token1");
        AddComponent<Clickable>(tokenHandle, MaskToken);
    }

    void ClickEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project