#pragma once

#include "CollisionCache.h"

namespace nc::physics
{
    /** Notify AutoComponents of collision events. */
    void NotifyCollisionEvents(registry_type* registry,
                               const std::vector<NarrowEvent>& physicsEvents,
                               const std::vector<NarrowEvent>& triggerEvents,
                               CollisionCache* cache);
}