#pragma once

#include "Ecs.h"
#include "ecs/ColliderSystem.h"
#include "CollisionUtility.h"

#include <cstdint>
#include <vector>

namespace nc
{
    namespace graphics { class FrameManager; }
    namespace job { class JobSystem; }
}

namespace nc::physics
{
    enum class CollisionEventType : uint8_t
    {
        Enter, Stay, Exit
    };

    /** Indices of two dynamic collider's positions in SoA.
     *  Produced by broad detection, consumed by narrow detection. */
    struct BroadDetectVsDynamicEvent
    {
        uint32_t first;
        uint32_t second;
    };

    /** Index of dynamic collider's SoA position and pointer to static
     *  collider tree entry. Produced by broad detection, consumed by
     *  narrow detection. */
    struct BroadDetectVsStaticEvent
    {
        uint32_t first;
        const ecs::StaticTreeEntry* second;
    };

    /** EntityHandles of two colliding objects. Produced by narrow detection,
     *  consumed by compare/notify. */
    struct NarrowDetectEvent
    {
        HandleTraits::handle_type first;
        HandleTraits::handle_type second;
    };

    /** @todo FindXXXEvents will notify immediately - do we want to delay this? */

    class CollisionSystem
    {
        public:
            CollisionSystem(ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem);

            void DoCollisionStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager* frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            ecs::ColliderSystem* m_colliderSystem;
            job::JobSystem* m_jobSystem;
            std::vector<DynamicEstimate> m_dynamicEstimates;
            std::vector<BroadDetectVsDynamicEvent> m_broadEventsVsDynamic;
            std::vector<BroadDetectVsStaticEvent> m_broadEventsVsStatic;
            std::vector<NarrowDetectEvent> m_currentCollisions;
            std::vector<NarrowDetectEvent> m_previousCollisions;

            void FetchEstimates();
            void BroadDetectVsDynamic();
            void BroadDetectVsStatic();
            void NarrowDetectVsDynamic();
            void NarrowDetectVsStatic();
            void FindEnterAndStayEvents() const;
            void FindExitEvents() const;
            void NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const;
            void Cleanup();
    };
} // namespace nc::physics