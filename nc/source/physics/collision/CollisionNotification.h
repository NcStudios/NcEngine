#pragma once

#include "CollisionCache.h"

namespace nc::physics
{
    /** Notify AutoComponents of collision events. */
    void NotifyCollisionEvents(registry_type* registry, CollisionCache* cache);
}