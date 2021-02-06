#include "GameScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "config/ProjectConfig.h"
#include "components/EdgePanCamera.h"
#include "components/DebugComponents.h"
#include "source/Prefabs.h"
#include "components/GamePiece.h"
#include "components/ClickHandler.h"
#include "components/PlayerConnection.h"

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
        nc::ui::Set(m_hud.get());

        // Player Network Instance
        auto playerHandle = CreateEntity("Player");
        AddComponent<project::PlayerConnection>(playerHandle, m_serverIP, projectConfig.userName);

        // Light
        auto lvHandle = CreateEntity(Vector3{-2.4f, 12.1f, 0.0f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle);
        auto lvHandle2 = CreateEntity(Vector3{12.1f, 14.5f, 7.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle2);
        auto lvHandle3 = CreateEntity(Vector3{4.1f, 14.5f, 3.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle3);

        //Camera
        auto camHandle = CreateEntity(Vector3{0.0f, 5.0f, 0.0f}, Quaternion::FromEulerAngles(1.3f, 0.0f, 0.0f), Vector3::One(), "Main Camera");
        auto camComponentPtr = AddComponent<Camera>(camHandle);
        camera::SetMainCamera(camComponentPtr);
        AddComponent<EdgePanCamera>(camHandle);
        AddComponent<ClickHandler>(camHandle);
        
        // Debug Controller
        auto debugHandle = CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), "Debug Controller");
        AddComponent<SceneReset>(debugHandle);
        AddComponent<Timer>(debugHandle);

        // Table
        prefab::Create(prefab::Resource::Table, Vector3{4.0f, -0.4f, 3.0f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), Vector3::Splat(7.5f), "Table");
    }
    
    void GameScene::Unload()
    {
        nc::ui::Set(nullptr);
        m_hud = nullptr;
        m_log = nullptr;
    }
} // end namespace project