#include "Worms.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "shared/ConstantRotation.h"
#include "shared/spawner/Spawner.h"
#include "MouseFollower.h"

#include <random>

namespace nc::sample
{
    void Worms::Load(registry_type* registry)
    {
        // Setup
        m_sceneHelper.Setup(registry, false);
        prefab::InitializeResources();

        // Camera
        auto cameraEntity = registry->Add<Entity>({.tag = "Main Camera"});
        auto camera = registry->Add<Camera>(cameraEntity);
        camera::SetMainCamera(camera);

        // Light
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

        auto lightHandle = registry->Add<Entity>({.tag = "Point Light"});
        registry->Add<PointLight>(lightHandle, lightProperties);
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