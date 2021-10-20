#include "ClickEvents.h"
#include "NcEngine.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/EdgePanCamera.h"
#include "shared/spawner/Spawner.h"
#include "Clickable.h"
#include "ClickHandler.h"

#include <functional>

namespace
{
    std::function<void(nc::LayerMask)> LayerSelectCallback = nullptr;
    const auto CoinLayer = nc::Entity::layer_type{1u};
    const auto TokenLayer = nc::Entity::layer_type{2u};
    const auto MaskNone = nc::LayerMask32None;
    const auto MaskAll = nc::LayerMask32All;
    const auto MaskCoin = nc::LayerMask32{1u};
    const auto MaskToken = nc::LayerMask32{2u};

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
    void ClickEvents::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();
        
        // Setup
        m_sceneHelper.Setup(engine, true, false, Widget);

        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -9.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<EdgePanCamera>(cameraHandle);
        engine->MainCamera()->Set(camera);
        auto clickHandler = registry->Add<ClickHandler>(cameraHandle, MaskAll, engine->Physics());
        LayerSelectCallback = std::bind(ClickHandler::SetLayer, clickHandler, std::placeholders::_1);

        // Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{-2.4f, 12.1f, 0.0f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{});

        auto lvHandle2 = registry->Add<Entity>({.position = Vector3{12.1f, 14.5f, 7.3f}, .tag = "Point Light 2"});
        registry->Add<PointLight>(lvHandle2, PointLightInfo{});

        auto lvHandle3 = registry->Add<Entity>({.position = Vector3{4.1f, 14.5f, 3.3f}, .tag = "Point Light 3"});
        registry->Add<PointLight>(lvHandle3, PointLightInfo{});

        // Objects
        prefab::Create(registry,
                       prefab::Resource::Table, 
                       {.position = Vector3{0.0f, -0.4f, 0.0f},
                       .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
                       .scale = Vector3::Splat(7.5f),
                       .tag = "Table"});
        
        // Coin Spawner
        SpawnBehavior behavior
        {
            .rotationOffset = Vector3{std::numbers::pi / 2.0f, 0.0f, 0.0f},
            .positionRandomRange = Vector3{4.2f, 0.0f, 2.8f},
            .layer = CoinLayer
        };
        
        auto spawnExtension = [registry, physicsSystem = engine->Physics()](Entity handle)
        {
            registry->Get<Transform>(handle)->SetScale(Vector3::Splat(2.0f));
            registry->Add<Clickable>(handle, registry->Get<Tag>(handle)->Value().data(), physicsSystem);
        };

        auto coinSpawnerHandle = registry->Add<Entity>({.tag = "Coin Spawner"});
        auto coinSpawner = registry->Add<Spawner>(coinSpawnerHandle, registry, prefab::Resource::Coin, behavior, spawnExtension);
        coinSpawner->Spawn(20);
        
        behavior.layer = TokenLayer;
        auto tokenSpawnerHandle = registry->Add<Entity>({.tag = "Token Spawner"});
        auto tokenSpawner = registry->Add<Spawner>(tokenSpawnerHandle, registry, prefab::Resource::Token, behavior, spawnExtension);
        tokenSpawner->Spawn(20);
    }

    void ClickEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project