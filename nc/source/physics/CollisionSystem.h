#pragma once

#include "Ecs.h"
#include "ecs/ColliderSystem.h"

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

    /** An estimated bounding volume and index mapping to the associated
     *  collider's position in the SoA representation. */
    struct DynamicEstimate
    {
        SphereCollider estimate;
        uint32_t index;
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

    /** Entitys of two colliding objects. Produced by narrow detection,
     *  consumed by compare/notify. */
    struct NarrowDetectEvent
    {
        EntityTraits::underlying_type first;
        EntityTraits::underlying_type second;
    };

    #ifdef NC_EDITOR_ENABLED
    struct CollisionMetrics
    {
        unsigned staticBroadChecks;
        unsigned staticNarrowChecks;
        unsigned dynamicBroadChecks;
        unsigned dynamicNarrowChecks;

        void Reset()
        {
            staticBroadChecks = 0u;
            staticNarrowChecks = 0u;
            dynamicBroadChecks = 0u;
            dynamicNarrowChecks = 0u;
        }
    };
    #endif

    /** @todo FindXXXEvents will notify immediately - do we want to delay this? */

    class CollisionSystem
    {
        public:
            CollisionSystem(ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem);

            void DoCollisionStep();
            void ClearState();

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
            void FindExitAndStayEvents() const;
            void FindEnterEvents() const;
            void NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const;
            void Cleanup();
        
        public:
            #ifdef NC_EDITOR_ENABLED
            inline static CollisionMetrics metrics;
            #endif
    };
} // namespace nc::physics