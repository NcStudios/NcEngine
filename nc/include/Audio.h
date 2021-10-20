#pragma once

#include "Entity.h"

namespace nc
{
    /** A listener must be registered for the AudioSystem to operate. The most recently
     *  registered listener's position will be used for spatial audio. A listener must not
     *  be destroyed, except on a scene change. Entity::Null() may be passed to disable
     *  audio. */
    void RegisterAudioListener(Entity entity);
}