#pragma once

#include "ecs/Entity.h"
#include "engine_module.h"

namespace nc
{
    struct audio_module : public engine_module
    {
        /** A listener must be registered for the AudioSystem to operate. The most recently
        *  registered listener's position will be used for spatial audio. A listener must not
        *  be destroyed, except on a scene change. Entity::Null() may be passed to disable
        *  audio. */
        virtual void register_listener(Entity entity) noexcept = 0;

        /** @todo replace with task/workload */
        virtual void update() = 0;
    };
}