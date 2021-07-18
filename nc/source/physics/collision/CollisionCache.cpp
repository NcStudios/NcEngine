#include "CollisionCache.h"

namespace
{
    using namespace nc::physics;

    void Clear(BroadPhaseCache& cache)
    {
        cache.physics.clear();
        cache.trigger.clear();
        cache.staticPhysics.clear();
        cache.staticTrigger.clear();
    }

    void Clear(NarrowPhaseCache& cache)
    {
        cache.physics.clear();
        cache.trigger.clear();
    }
}

namespace nc::physics
{
    bool operator ==(const BroadDetectVsStaticEvent& lhs, const BroadDetectVsStaticEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second);
    }

    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    void UpdatePreviousEvents(CollisionCache* cache)
    {
        cache->previousNarrow = std::move(cache->narrow);
        ::Clear(cache->narrow);
        ::Clear(cache->broad);
        cache->estimates.clear();
    }

    void Clear(CollisionCache* cache)
    {
        ::Clear(cache->broad);
        ::Clear(cache->narrow);
        ::Clear(cache->previousNarrow);
        cache->estimates.clear();
        cache->manifolds.clear();
    }
} // namespace nc::physics