#pragma once

#include "PairCache.h"
#include "physics/PhysicsPipelineTypes.h"

namespace nc::physics
{
    class ManifoldCache : public PairCache<ManifoldCache, Manifold>
    {
        public:
            bool PairEqual(Entity a, Entity b, const Manifold& manifold)
            {
                return (a == manifold.event.first && b == manifold.event.second) ||
                       (a == manifold.event.second && b == manifold.event.first);
            }

            void AddToExisting(Manifold* existing, CollisionEventType, const Contact& contact)
            {
                existing->AddContact(contact);
            }

            auto ConstructNew(Entity a, Entity b, CollisionEventType type, const Contact& contact) -> Manifold
            {
                return Manifold
                {
                    .event{.first = a, .second = b, .eventType = type},
                    .contacts = {contact}
                };
            }

            auto Hash(const Manifold& manifold) -> uint32_t
            {
                return PairCache<ManifoldCache, Manifold>::Hash(manifold.event.first, manifold.event.second);
            }

            void AddToRemoved(Entity a, Entity b)
            {
                m_removed.emplace_back(a, b);
            }

            auto GetRemoved() -> std::span<const std::pair<Entity, Entity>>
            {
                return m_removed;
            }
            
            void ClearRemoved()
            {
                m_removed.clear();
            }

        private:
            std::vector<std::pair<Entity, Entity>> m_removed;
    };
}