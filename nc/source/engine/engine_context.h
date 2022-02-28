#pragma once

#include "application.h"
#include "assets/AssetServices.h"
#include "audio/AudioSystemImpl.h"
#include "camera/MainCameraImpl.h"
#include "config/ConfigInternal.h"
#include "context_stub.h"
#include "debug/Utils.h"
#include "ecs/ParticleEmitterSystem.h"
#include "ecs/PointLightSystem.h"
#include "ecs/Registry.h"
#include "graphics/resources/EnvironmentImpl.h"
#include "math/Random.h"
#include "NcEngine.h"
#include "physics/PhysicsSystemImpl.h"
#include "scene/SceneSystemImpl.h"
#include "time/NcTime.h"

namespace nc
{
    struct engine_context
    {
        std::unique_ptr<Registry> registry;
        std::unique_ptr<camera::MainCameraImpl> mainCamera;
        std::unique_ptr<application> application;
        std::unique_ptr<ecs::ParticleEmitterSystem> particleSystem;
        std::unique_ptr<ecs::PointLightSystem> pointLightSystem;
        std::unique_ptr<physics::PhysicsSystemImpl> physicsSystem;
        std::unique_ptr<scene::SceneSystemImpl> sceneSystem;
        std::unique_ptr<time::Time> time;
        std::unique_ptr<audio::AudioSystemImpl> audioSystem;
        std::unique_ptr<nc::EnvironmentImpl> environment;
        std::unique_ptr<nc::Random> random;
        std::vector<std::unique_ptr<context_stub>> stubs;
    };

    auto build_engine_context(engine_init_flags flags) -> engine_context;
    void clear(engine_context* context);
}