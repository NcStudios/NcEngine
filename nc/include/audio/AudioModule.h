#pragma once

#include "ecs/Entity.h"
#include "EngineModule.h"

namespace nc
{
    struct AudioModule : public EngineModule
    {
        /** A listener must be registered for the AudioSystem to operate. The most recently
        *  registered listener's position will be used for spatial audio. A listener must not
        *  be destroyed, except on a scene change. Entity::Null() may be passed to disable
        *  audio. */
        virtual void RegisterListener(Entity entity) noexcept = 0;

        /** @todo replace with task/workload */
        virtual void Update() = 0;
    };
}