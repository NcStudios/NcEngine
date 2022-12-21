#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/module/Module.h"

namespace nc::audio
{
/** @brief Audio module interface.
 *  
 *  Component Access:
 *      Write: AudioSource
 *      Read: Transform
*/
struct NcAudio : public Module
{
    /**
     * @brief Register an object to be the reference point audio.
     * 
     * A listener must be registered for the audio module to operate. A listener
     * should not be destroyed while it is registered, except on a scene change.
     * A new Entity or Entity::Null() may be passed to unregister the current
     * listener.
     * 
     * @param entity An Entity to use as the listener.
     */
    virtual void RegisterListener(Entity entity) noexcept = 0;
};
} // namespace nc::audio
