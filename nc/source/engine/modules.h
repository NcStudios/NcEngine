#pragma once

#include "audio/audio_module.h"
#include "ecs/ParticleEmitterSystem.h"
#include "graphics/graphics_module.h"
#include "physics/physics_module.h"

namespace nc
{
    struct modules
    {
        std::unique_ptr<graphics_module> graphicsModule;
        std::unique_ptr<ecs::ParticleEmitterSystem> particleSystem;
        std::unique_ptr<physics_module> physicsModule;
        std::unique_ptr<audio_module> audioModule;
    };
}