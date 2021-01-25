#include "DemoScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "config/ProjectConfig.h"
#include "components/CamController.h"
#include "components/DebugComponents.h"
#include "source/Prefabs.h"
#include "components/GamePiece.h"
#include "components/ClickHandler.h"
#include "graphics/materials/Material.h"
#include "graphics/techniques/TechniqueType.h"

using namespace nc;

namespace project
{
    void DemoScene::Load()
    {
        project::config::ProjectConfig projectConfig;
        projectConfig.Load();

        m_log = std::make_unique<project::log::GameLog>();
        m_hud = std::make_unique<project::ui::Hud>(m_log.get(), projectConfig);
        nc::ui::UI::Set(m_hud.get());

        // Lights
        auto lvHandle = Ecs::CreateEntity({-2.4f, 12.1f, 0.0f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle);
        auto lvHandle2 = Ecs::CreateEntity({12.1f, 14.5f, 7.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle2);
        auto lvHandle3 = Ecs::CreateEntity({4.1f, 14.5f, 3.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle3);

        //CamController
        auto camHandle = Ecs::CreateEntity({0.0f, 0.0f, -10.0f}, Quaternion::Identity(), Vector3::One(), "Main Camera");
        auto camComponentPtr = Ecs::AddComponent<Camera>(camHandle);
        camera::MainCamera::Set(camComponentPtr);
        Ecs::AddComponent<CamController>(camHandle);
        Ecs::AddComponent<ClickHandler>(camHandle);

        // Debug Controller
        auto debugHandle = Ecs::CreateEntity("Debug Controller");
        Ecs::AddComponent<SceneReset>(debugHandle);
        Ecs::AddComponent<Timer>(debugHandle);

        auto scale = Vector3::One() * 0.1f;
        auto boarHandle = prefab::Create(prefab::Resource::Boar, Vector3::One(), Quaternion::Identity(), Vector3::One() * 2, "Boar");
        Ecs::AddComponent<Collider>(boarHandle, scale);

        const auto scaleFactor = 2;
        auto materialProperties = graphics::MaterialProperties{};
        // Table
        const std::vector<std::string> tableTextures = {"project//Textures//DiningRoomTable_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png",  "project//Textures//DiningRoomTable_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
        graphics::Material tableMaterial =graphics::Material::CreateMaterial<graphics::TechniqueType::PhongShadingTechnique>(tableTextures, materialProperties);
        auto tableHandle = Ecs::CreateEntity({2.0f  * scaleFactor, -0.4f, 1.5f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), Vector3::One() * 7.5, "Table Piece");
        auto tableMesh = graphics::Mesh{"project//Models//DiningRoomTable.fbx"};
        Ecs::AddComponent<Renderer>(tableHandle, tableMesh, tableMaterial);

        // // Piece
        prefab::Create(prefab::Resource::Boar, {0.2f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Boar Piece");
        prefab::Create(prefab::Resource::Cattle, {0.4f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Cattle Piece");
        prefab::Create(prefab::Resource::Coal, {0.2f * scaleFactor, 0.0f, 0.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Coal Piece");    
        prefab::Create(prefab::Resource::CoinOne, {0.8f * scaleFactor, 0.0f, 0.8f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "OneCoin Piece");
        prefab::Create(prefab::Resource::CoinTen, {1.0f * scaleFactor, 0.0f, 0.8f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "TenCoin Piece");
        prefab::Create(prefab::Resource::CoinTwo, {0.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "TwoCoin Piece");
        prefab::Create(prefab::Resource::Dog, Vector3::Zero(), Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Dog Piece");
        prefab::Create(prefab::Resource::Donkey, {0.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Donkey Piece");
        prefab::Create(prefab::Resource::Grain, {0.6f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Grain Piece");
        prefab::Create(prefab::Resource::Ruby, {0.4f * scaleFactor, 0.0f, 0.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Ruby Piece");
        prefab::Create(prefab::Resource::Sheep, {1.0f * scaleFactor, 0.0f, 0.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Sheep Piece");
        prefab::Create(prefab::Resource::StartingPlayerPiece, {1.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Starting Player Piece");
        prefab::Create(prefab::Resource::Stone, {0.6f * scaleFactor, 0.0f, 0.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Stone Piece");
        prefab::Create(prefab::Resource::Vegetable, {0.8f * scaleFactor, 0.0f, 0.2f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Vegetable Piece");
        prefab::Create(prefab::Resource::Wood, {0.8f * scaleFactor, 0.0f, 0.0f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::One() * scaleFactor, "Wood Piece");

        // Player Board
        prefab::Create(prefab::Resource::PlayerBoard, {3.2f * scaleFactor, 0.0f, 0.3f * scaleFactor}, Quaternion::FromEulerAngles(1.5708f, 1.5708f, 0.0f), Vector3::One() * scaleFactor * 0.1f, "PLayerBoard Piece");
    }

    void DemoScene::Unload()
    {
        nc::ui::UI::Set(nullptr);
        m_hud = nullptr;
        m_log = nullptr;
    }
} //end namespace project