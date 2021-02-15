#pragma once

#include "Ecs.h"

#include <cstdint>
#include <vector>

namespace nc::physics
{
    enum class CollisionEvent : uint8_t
    {
        Enter = 0u, Stay = 1u, Exit = 2u
    };

    /** @note Possible problem here:
     *  Frame 1: c1 collided with -> destroyed -> c2 created at c1 address
     *  Frame 2: c2 collision events incorrectly based on past c1 state
     *  Solution: Can maybe keep handles, but more likely, we will want
     *  ownership of colliders in physics somewhere. */
    struct CollisionData
    {
        Collider* first;
        Collider* second;
    };

    bool operator ==(const CollisionData& lhs, const CollisionData& rhs);

    class CollisionSystem
    {
        public:
            void DoCollisionStep(const std::vector<Collider*>& colliders);
            void BuildCurrentFrameState(const std::vector<Collider*>& colliders);
            void CompareFrameStates() const;
            void NotifyCollisionEvent(const CollisionData& data, CollisionEvent type) const;
            void ClearState();

        private:
            std::vector<CollisionData> m_currentCollisions;
            std::vector<CollisionData> m_previousCollisions;
    };
}