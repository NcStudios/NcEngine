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
            return (a == manifold.Event().first && b == manifold.Event().second) ||
                    (a == manifold.Event().second && b == manifold.Event().first);
        }

        void AddToExisting(Manifold* existing, CollisionEventType, bool, const Contact& contact)
        {
            existing->AddContact(contact);
        }

        auto ConstructNew(Entity a, Entity b, CollisionEventType type, bool stickyContacts, const Contact& contact) -> Manifold
        {
            return Manifold{a, b, type, stickyContacts, contact};
        }

        auto Hash(const Manifold& manifold) -> uint32_t
        {
            return PairCache<ManifoldCache, Manifold>::Hash(manifold.Event().first, manifold.Event().second);
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
} // namespace nc::physics
