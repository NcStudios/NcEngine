#include "GameScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "config/ProjectConfig.h"
#include "CamController.h"
#include "DebugComponents.h"
#include "source/Prefabs.h"
#include "GamePiece.h"
#include "ClickHandler.h"
#include "PlayerConnection.h"

using namespace nc;

namespace project
{
    GameScene::GameScene(std::string serverIP)
        : m_serverIP{std::move(serverIP)}
    {
    }

    void GameScene::Load()
    {
        project::config::ProjectConfig projectConfig;
        projectConfig.Load();

        m_log = std::make_unique<project::log::GameLog>();
        m_hud = std::make_unique<project::ui::Hud>(m_log.get(), projectConfig);
        nc::ui::UI::Set(m_hud.get());

        // Player Network Instance
        auto playerHandle = Ecs::CreateEntity("Player");
        Ecs::AddComponent<project::PlayerConnection>(playerHandle, m_serverIP, projectConfig.userName);

        // Light
        auto lvHandle = Ecs::CreateEntity({-2.4f, 12.1f, 0.0f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle);

        // Light
        auto lvHandle2 = Ecs::CreateEntity({12.1f, 14.5f, 7.3f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle2);

        // Light
        auto lvHandle3 = Ecs::CreateEntity({4.1f, 14.5f, 3.3f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
        Ecs::AddComponent<PointLight>(lvHandle3);

        //CamController
        auto camHandle = Ecs::CreateEntity({0.0f, 5.0f, 0.0f}, {1.3f, 0.0f, 0.0f}, Vector3::Zero(), "Main Camera");
        auto camComponentPtr = Ecs::AddComponent<Camera>(camHandle);
        camera::MainCamera::Set(camComponentPtr);
        Ecs::AddComponent<CamController>(camHandle);
        Ecs::AddComponent<ClickHandler>(camHandle);
        
        // Debug Controller
        auto debugHandle = Ecs::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
        Ecs::AddComponent<SceneReset>(debugHandle);
        Ecs::AddComponent<Timer>(debugHandle);

        const auto scaleFactor = 2;

        // Table
        const std::vector<std::string> tableTextures = {"project//Textures//DiningRoomTable_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png",  "project//Textures//DiningRoomTable_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
        graphics::Material tableMaterial = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, tableTextures};
        auto tableMesh = graphics::Mesh{"project//Models//DiningRoomTable.fbx"};
        auto tableHandle = Ecs::CreateEntity({2.0f  * scaleFactor, -0.4f, 1.5f * scaleFactor}, {1.5708f, 0.0f, 1.5708f}, Vector3::One() * 7.5, "Table Piece");
        Ecs::AddComponent<Renderer>(tableHandle, tableMesh, tableMaterial);
    }
    
    void GameScene::Unload()
    {
        nc::ui::UI::Set(nullptr);
        m_hud = nullptr;
        m_log = nullptr;
    }
} // end namespace project