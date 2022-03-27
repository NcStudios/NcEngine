#include "Worms.h"
#include "NcEngine.h"
#include "shared/FreeComponents.h"
#include "shared/spawner/Spawner.h"

#include <random>

namespace nc::sample
{
    void Worms::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();

        // Setup
        m_sceneHelper.Setup(engine, false, false);
        prefab::InitializeResources();

        // Camera
        auto cameraEntity = registry->Add<Entity>({.tag = "Main Camera"});
        auto camera = registry->Add<Camera>(cameraEntity);
        engine->Graphics()->SetCamera(camera);

        // Window
        window::SetClearColor({0.05f, 0.05f, 0.05f, 1.0f});

        // Light
        auto lightOneProperties = PointLightInfo
        {
            .pos = Vector3{0.05f, 3.0f, -5.0f},
            .ambient = Vector3{0.3f, 0.3f, 0.3f},
            .diffuseColor = Vector3{0.3, 0.3, 0.3},
            .diffuseIntensity = 1200.0f
        };

        auto lightHandle = registry->Add<Entity>({.tag = "Point Light"});
        registry->Add<PointLight>(lightHandle, lightOneProperties);
        registry->Add<FrameLogic>(lightHandle, InvokeFreeComponent<MouseFollower>{lightHandle, registry});

        // Worm Spawner
        SpawnBehavior spawnBehavior
        {
            .minPosition = Vector3{-15.0f, -15.0f, 10.0f},
            .maxPosition = Vector3{15.0f, 15.0f, 40.0f},
            .minRotation = Vector3::Zero(),
            .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f),
            .rotationAxis = Vector3::One(),
            .rotationTheta = 1.0f
        };

        auto spawnerHandle = registry->Add<Entity>({.tag = "Spawner"});
        auto spawner = registry->Add<Spawner>(spawnerHandle, engine->Random(), prefab::Resource::Worm, spawnBehavior);
        registry->Add<FrameLogic>(spawnerHandle, InvokeFreeComponent<Spawner>{});
        spawner->Spawn(registry, 40u);
    }

    void Worms::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project