#pragma once

#include "PairCache.h"
#include "physics/PhysicsPipelineTypes.h"

namespace nc::physics
{
    class TriggerCache : public PairCache<TriggerCache, NarrowEvent, Entity, 256u>
    {
        public:
            bool PairEqual(Entity a, Entity b, NarrowEvent event)
            {
                return (a == event.first && b == event.second) ||
                       (a == event.second && b == event.first);
            }

            void AddToExisting(NarrowEvent* existing, CollisionEventType)
            {
                existing->state = NarrowEvent::State::Persisting;
            }

            auto ConstructNew(Entity a, Entity b, CollisionEventType type)
            {
                return NarrowEvent{ .first = a, .second = b, .eventType = type};
            }

            auto Hash(const NarrowEvent& event) -> uint32_t
            {
                return PairCache<TriggerCache, NarrowEvent, Entity, 256u>::Hash(event.first, event.second);
            }
    };
}