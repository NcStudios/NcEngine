#include "ClickEvents.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/EdgePanCamera.h"
#include "shared/spawner/Spawner.h"
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
            ImGui::Text("-move camera by edge panning");
            ImGui::Text("-zoom with mouse wheel");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void ClickEvents::Load()
    {
        // Setup
        m_sceneHelper.Setup(true, false, Widget);

        auto cameraHandle = CreateEntity({.position = Vector3{0.0f, 6.1f, -9.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = AddComponent<EdgePanCamera>(cameraHandle);
        camera::SetMainCamera(camera);
        auto clickHandler = AddComponent<ClickHandler>(cameraHandle, MaskAll);
        LayerSelectCallback = std::bind(ClickHandler::SetLayer, clickHandler, std::placeholders::_1);

        // Lights
        auto lvHandle = CreateEntity({.position = Vector3{-2.4f, 12.1f, 0.0f}, .tag = "Point Light"});
        AddComponent<PointLight>(lvHandle);
        auto lvHandle2 = CreateEntity({.position = Vector3{12.1f, 14.5f, 7.3f}, .tag = "Point Light"});
        AddComponent<PointLight>(lvHandle2);
        auto lvHandle3 = CreateEntity({.position = Vector3{4.1f, 14.5f, 3.3f}, .tag = "Point Light"});
        AddComponent<PointLight>(lvHandle3);

        // Objects
        prefab::Create(prefab::Resource::Table, 
                       {.position = Vector3{0.0f, -0.4f, 0.0f},
                       .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
                       .scale = Vector3::Splat(7.5f),
                       .tag = "Table"});
        
        // Coin Spawner
        SpawnBehavior behavior
        {
            .rotationOffset = Vector3{math::Pi / 2.0f, 0.0f, 0.0f},
            .positionRandomRange = Vector3{4.2f, 0.0f, 2.8f}
        };
        
        auto coinExtension = [](EntityHandle handle)
        {
            GetComponent<Transform>(handle)->SetScale(Vector3::Splat(2.0f));
            AddComponent<Clickable>(handle, MaskCoin);
        };

        auto coinSpawnerHandle = CreateEntity({.tag = "Coin Spawner"});
        auto coinSpawner = AddComponent<Spawner>(coinSpawnerHandle, prefab::Resource::Coin, behavior, coinExtension);
        coinSpawner->Spawn(20);

        // Token Spawner
        auto tokenExtension = [](EntityHandle handle)
        {
            GetComponent<Transform>(handle)->SetScale(Vector3::Splat(2.0f));
            AddComponent<Clickable>(handle, MaskToken);
        };
        
        auto tokenSpawnerHandle = CreateEntity({.tag = "Token Spawner"});
        auto tokenSpawner = AddComponent<Spawner>(tokenSpawnerHandle, prefab::Resource::Token, behavior, tokenExtension);
        tokenSpawner->Spawn(20);
    }

    void ClickEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project