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

        // Window
        window::SetClearColor({0.05f, 0.05f, 0.05f, 1.0f});

        // Light
        auto lightOneProperties = vulkan::PointLightInfo
        {
            .pos = Vector4::Zero(),
            .ambient = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
            .diffuseColor = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
            .specularColor = Vector4{1.0, 1.0f, 1.0f, 1.0f},
            .diffuseIntensity = 20.0f,
            .attConst = 1.0f,
            .attLin = 0.0014f,
            .attQuad = 0.000007f
        };

        // Light
        auto lightTwoProperties = vulkan::PointLightInfo
        {
            .pos = Vector4::Splat(5.0f),
            .ambient = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
            .diffuseColor = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
            .specularColor = Vector4{1.0, 1.0f, 1.0f, 1.0f},
            .diffuseIntensity = 20.0f,
            .attConst = 1.0f,
            .attLin = 0.0014f,
            .attQuad = 0.000007f
        };

        auto lightHandle = registry->Add<Entity>({.tag = "Point Light"});
        registry->Add<vulkan::PointLight>(lightHandle, lightOneProperties);
        registry->Add<MouseFollower>(lightHandle, registry);

        auto lightTwoHandle = registry->Add<Entity>({.tag = "Point Light"});
        registry->Add<vulkan::PointLight>(lightTwoHandle, lightTwoProperties);
        registry->Add<MouseFollower>(lightTwoHandle, registry);

        // Worm Spawner
        SpawnBehavior spawnBehavior
        {
            .positionOffset = Vector3{0.0f, 0.0f, 25.0f},
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
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