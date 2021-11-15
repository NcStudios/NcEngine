#include "Worms.h"
#include "NcEngine.h"
#include "shared/ConstantRotation.h"
#include "shared/spawner/Spawner.h"
#include "MouseFollower.h"

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
        engine->MainCamera()->Set(camera);

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
        registry->Add<MouseFollower>(lightHandle, registry);

        // Worm Spawner
        SpawnBehavior spawnBehavior
        {
            .positionOffset = Vector3{0.0f, 0.0f, 25.0f},
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(std::numbers::pi / 2.0f),
            .rotationAxisRandomRange = Vector3::One(),
            .thetaRandomRange = 1.0f
        };

        auto spawnerHandle = registry->Add<Entity>({.tag = "Spawner"});
        auto spawner = registry->Add<Spawner>(spawnerHandle, registry, prefab::Resource::Worm, spawnBehavior);
        spawner->Spawn(40u);
    }

    void Worms::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project