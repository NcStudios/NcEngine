#pragma once

#include "CollisionCache.h"

namespace nc { class Registry; }

namespace nc::physics
{
    /** Notify AutoComponents of collision events. */
    void NotifyCollisionEvents(Registry* registry,
                               const std::vector<NarrowEvent>& physicsEvents,
                               const std::vector<NarrowEvent>& triggerEvents,
                               CollisionCache* cache);
}