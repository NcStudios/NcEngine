#pragma once

#include "Ecs.h"
#include "ColliderSystem.h"

#include <cstdint>
#include <vector>

namespace nc::graphics { class FrameManager; }

namespace nc::physics
{
    enum class CollisionEventType : uint8_t
    {
        Enter = 0u, Stay = 1u, Exit = 2u
    };

    /** @note Possible problem here:
     *  Frame 1: c1 collided with -> destroyed -> c2 created at c1 address
     *  Frame 2: c2 collision events incorrectly based on past c1 state
     *  Solution: Can maybe keep handles, but more likely, we will want
     *  ownership of colliders in physics somewhere. */
    struct CollisionEvent
    {
        Collider* first;
        Collider* second;
    };

    bool operator ==(const CollisionEvent& lhs, const CollisionEvent& rhs);

    class CollisionSystem
    {
        public:
            CollisionSystem();
            void DoCollisionStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager& frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            ColliderSystem m_colliderSystem;
            std::vector<Collider*> m_dynamicColliders;
            std::vector<Collider*> m_staticColliders;
            std::vector<CollisionEvent> m_broadPhaseEvents;
            std::vector<CollisionEvent> m_currentStepEvents;
            std::vector<CollisionEvent> m_previousStepEvents;

            void FetchColliders();
            void BroadPhase();
            void NarrowPhase();
            void CompareFrameStates() const;
            void NotifyCollisionEvent(const CollisionEvent& data, CollisionEventType type) const;
            void Cleanup();
    };
}