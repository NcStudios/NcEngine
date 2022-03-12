#pragma once

#include "ecs/Entity.h"
#include "Module.h"

namespace nc
{
    /** @brief Interface for general audio functionality. */
    struct AudioModule : public Module
    {
        /**
         * @brief Register an object to be the reference point audio.
         * 
         * A listener must be registered for the AudioModule to operate. A listener
         * should not be destroyed while it is registered, except on a scene change.
         * A new Entity or Entity::Null() may be passed to unregister the current
         * listener.
         * 
         * @param entity An Entity to use as the listener.
         */
        virtual void RegisterListener(Entity entity) noexcept = 0;
    };
}