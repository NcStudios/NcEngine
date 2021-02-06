#include "MenuScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "config/ProjectConfig.h"
#include "components/DebugComponents.h"
#include "project/components/ConstantRotation.h"
#include "project/components/MouseFollower.h"
#include "source/Prefabs.h"
#include "graphics/Material.h"

#include <cstdlib>
#include <random>

using namespace nc;

namespace project
{
    void MenuScene::Load()
    {
        project::config::ProjectConfig projectConfig;
        projectConfig.Load();

        m_ui = std::make_unique<project::ui::MainMenuUI>(projectConfig);
        nc::ui::Set(m_ui.get());

        prefab::InitializeResources();

        // Light
        auto lvHandle = CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), "Point Light");
        auto pointLight = AddComponent<PointLight>(lvHandle);
        auto lightProperties = PointLight::Properties
        {
            .pos = Vector3::Zero(),
            .ambient = Vector3{0.443f, 0.412f, 0.412f},
            .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
            .diffuseIntensity = 3.56,
            .attConst = 0.0f,
            .attLin = 0.05f,
            .attQuad = 0.0f
        };
        pointLight->Set(lightProperties);
        AddComponent<project::MouseFollower>(lvHandle);

        // CamController
        auto camHandle = CreateEntity("Main Camera");
        auto camComponentPtr = AddComponent<Camera>(camHandle);
        camera::SetMainCamera(camComponentPtr);
        
        // Debug Controller
        auto debugHandle = CreateEntity("Debug Controller");
        AddComponent<SceneReset>(debugHandle);
        AddComponent<Timer>(debugHandle);

        // Worms
        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_real_distribution<float> randPos(-15.0f, 15.0f);
        std::uniform_real_distribution<float> randRot(0.0f, 3.14159f);

        for(size_t i = 0u; i < 40; ++i)
        {
            auto pos = Vector3{randPos(gen), randPos(gen), randPos(gen) + 25.0f};
            auto rot = Quaternion::FromEulerAngles(randRot(gen), randRot(gen), randRot(gen));
            auto wormHandle = prefab::Create(prefab::Resource::Worm, pos, rot, Vector3::Splat(0.75f), "worm");
            AddComponent<project::ConstantRotation>(wormHandle, Vector3{0, 1, 0}, 0.1f);
        }
    }

    void MenuScene::Unload()
    {
        nc::ui::Set(nullptr);
        m_ui = nullptr;
    }
} // end namespace project