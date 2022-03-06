#pragma once

#include "audio_module.h"
#include "ecs/ParticleEmitterSystem.h"
#include "ecs/Registry.h"
#include "graphics/graphics_module.h"
#include "math/Random.h"
#include "nc_engine.h"
#include "physics/physics_module.h"
#include "scene/SceneSystemImpl.h"

namespace nc
{
    struct modules
    {
        std::unique_ptr<Registry> registry;
        std::unique_ptr<graphics_module> graphicsModule;
        std::unique_ptr<ecs::ParticleEmitterSystem> particleSystem;
        std::unique_ptr<physics_module> physicsModule;
        std::unique_ptr<scene::SceneSystemImpl> sceneSystem;
        std::unique_ptr<audio_module> audioModule;
        std::unique_ptr<nc::Random> random;
    };
}