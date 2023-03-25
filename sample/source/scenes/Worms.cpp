#include "Worms.h"
#include "shared/FreeComponents.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/graphics/NcGraphics.h"

#include <random>

namespace nc::sample
{
Worms::Worms(SampleUI* ui)
{
    ui->SetWidgetCallback(nullptr);
}

void Worms::Load(Registry* registry, ModuleProvider modules)
{
    // Setup
    prefab::InitializeResources();

    // Camera
    auto cameraEntity = registry->Add<Entity>({.tag = "Main Camera"});
    auto camera = registry->Add<graphics::Camera>(cameraEntity);
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Light
    auto lightHandle = registry->Add<Entity>({.tag = "Point Light"});
    registry->Add<graphics::PointLight>(lightHandle, Vector3{0.3f, 0.3f, 0.3f}, Vector3{0.3f, 0.3f, 0.3f}, 1200.0f);
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
    auto spawner = registry->Add<Spawner>(spawnerHandle, modules.Get<Random>(), prefab::Resource::Worm, spawnBehavior);
    registry->Add<FrameLogic>(spawnerHandle, InvokeFreeComponent<Spawner>{});
    spawner->Spawn(registry, 40u);
}
}
