#pragma once

#include "audio/AudioModule.h"
#include "ecs/ParticleEmitterSystem.h"
#include "graphics/GraphicsModule.h"
#include "physics/PhysicsModule.h"

namespace nc
{
    struct Modules
    {
        std::unique_ptr<GraphicsModule> graphicsModule;
        std::unique_ptr<ecs::ParticleEmitterSystem> particleSystem;
        std::unique_ptr<PhysicsModule> physicsModule;
        std::unique_ptr<AudioModule> audioModule;
    };
}