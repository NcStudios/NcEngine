#pragma once

#include "Entity.h"
#include "SystemBase.h"

namespace nc
{
    class AudioSystem : public SystemBase
    {
        public:
            /** A listener must be registered for the AudioSystem to operate. The most recently
            *  registered listener's position will be used for spatial audio. A listener must not
            *  be destroyed, except on a scene change. Entity::Null() may be passed to disable
            *  audio. */
            virtual void RegisterListener(Entity entity) = 0;
    };
}