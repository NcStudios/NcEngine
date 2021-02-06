#include "DemoScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "config/ProjectConfig.h"
#include "source/Prefabs.h"
#include "components/EdgePanCamera.h"
#include "components/DebugComponents.h"
#include "components/GamePiece.h"
#include "components/ClickHandler.h"
#include "components/WasdController.h"
#include "components/CollisionLogger.h"

using namespace nc;

namespace project
{
    void DemoScene::Load()
    {
        project::config::ProjectConfig projectConfig;
        projectConfig.Load();

        m_log = std::make_unique<project::log::GameLog>();
        m_hud = std::make_unique<project::ui::Hud>(m_log.get(), projectConfig);
        nc::ui::Set(m_hud.get());

        // Lights
        auto lvHandle = CreateEntity(Vector3{-2.4f, 12.1f, 0.0f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle);
        auto lvHandle2 = CreateEntity(Vector3{12.1f, 14.5f, 7.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle2);
        auto lvHandle3 = CreateEntity(Vector3{4.1f, 14.5f, 3.3f}, Quaternion::Identity(), Vector3::One(), "Point Light");
        AddComponent<PointLight>(lvHandle3);

        //Camera
        auto camHandle = CreateEntity(Vector3{0.0f, 0.0f, -10.0f}, Quaternion::Identity(), Vector3::One(), "Main Camera");
        auto camComponentPtr = AddComponent<Camera>(camHandle);
        camera::SetMainCamera(camComponentPtr);
        AddComponent<EdgePanCamera>(camHandle);
        AddComponent<ClickHandler>(camHandle);

        // Debug Controller
        auto debugHandle = CreateEntity("Debug Controller");
        AddComponent<SceneReset>(debugHandle);
        AddComponent<Timer>(debugHandle);

        // Objects
        prefab::Create(prefab::Resource::Table, Vector3{4.0f, -0.4f, 3.0f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), Vector3::Splat(7.5f), "Table");
        prefab::Create(prefab::Resource::Coin, Vector3{1.6f, 0.0f, 1.6f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::Splat(2.0f), "Coin");
        
        auto colliderScale = Vector3::Splat(0.1f);
        auto token1 = prefab::Create(prefab::Resource::Token, Vector3{2.0f, 0.0f, 0.4f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::Splat(2.0f), "Token1");
        AddComponent<Collider>(token1, colliderScale);
        AddComponent<project::WasdController>(token1, 0.5f);
        AddComponent<project::CollisionLogger>(token1);

        auto token2 = prefab::Create(prefab::Resource::Token, Vector3{6.0f, 0.0f, 0.4f}, Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f), Vector3::Splat(2.0f), "Token2");
        AddComponent<Collider>(token2, colliderScale);
        AddComponent<project::CollisionLogger>(token2);
    }

    void DemoScene::Unload()
    {
        nc::ui::Set(nullptr);
        m_hud = nullptr;
        m_log = nullptr;
    }
} //end namespace project