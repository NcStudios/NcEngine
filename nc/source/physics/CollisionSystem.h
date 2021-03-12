#pragma once

#include "Ecs.h"
#include "ColliderSystem.h"
#include "CollisionUtility.h"

#include <cstdint>
#include <vector>

namespace nc::graphics { class FrameManager; }

namespace nc::job { class ThreadPool; }

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
        const StaticTreeEntry* second;
    };

    /** EntityHandles of two colliding objects. Produced by narrow detection,
     *  consumed by compare/notify. */
    struct NarrowDetectEvent
    {
        EntityHandle::Handle_t first;
        EntityHandle::Handle_t second;
    };

    class CollisionSystem
    {
        public:
            CollisionSystem(float worldspaceExtent, job::ThreadPool* threadPool);
            void DoCollisionStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager* frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            ColliderSystem m_colliderSystem;
            std::vector<DynamicEstimate> m_dynamicEstimates;
            std::vector<DynamicActual> m_dynamicVolumes;
            std::vector<BroadDetectVsDynamicEvent> m_broadEventsVsDynamic;
            std::vector<BroadDetectVsStaticEvent> m_broadEventsVsStatic;
            std::vector<NarrowDetectEvent> m_currentCollisions;
            std::vector<NarrowDetectEvent> m_previousCollisions;
            job::ThreadPool* m_threadPool;

            void FetchEstimates();
            void BroadDetectVsDynamic();
            void BroadDetectVsStatic();
            void NarrowDetectVsDynamic();
            void NarrowDetectVsStatic();
            // may want to delay sending
            void FindEnterAndStayEvents() const;
            void FindExitEvents() const;
            void NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const;
            void Cleanup();
    };
} // namespace nc::physics