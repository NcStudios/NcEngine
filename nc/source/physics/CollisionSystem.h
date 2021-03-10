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
        Enter, Stay, Exit
    };

    // Produced by fetch, consumed by broad detection
    struct EstimateData
    {
        DirectX::BoundingSphere volumeEstimate;
        uint32_t index;
    };

    // Produced by broad detection, consumed by narrow detection
    struct BroadDetectEvent
    {
        uint32_t first;
        uint32_t second;
    };

    // Produced by narrow detection, consumed by compare/notify
    struct NarrowDetectEvent
    {
        EntityHandle::Handle_t first;
        EntityHandle::Handle_t second;
    };

    class CollisionSystem
    {
        public:
            CollisionSystem();
            void DoCollisionStep();
            void ClearState();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidgets(graphics::FrameManager* frameManager); // hacky solution until widgets are a real thing
            #endif

        private:
            ColliderSystem m_colliderSystem;
            std::vector<EstimateData> m_dynamicEstimates;
            std::vector<EstimateData> m_staticEstimates;
            std::vector<BroadDetectEvent> m_estimateOverlapDynamicVsDynamic;
            std::vector<BroadDetectEvent> m_estimateOverlapDynamicVsStatic;
            std::vector<NarrowDetectEvent> m_currentCollisions;
            std::vector<NarrowDetectEvent> m_previousCollisions;

            void FetchEstimates();
            void BroadDetection();
            void NarrowDetection();
            void CompareToPreviousStep() const;
            void NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const;
            void Cleanup();
    };
} // namespace nc::physics