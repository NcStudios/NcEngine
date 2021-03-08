#include "CollisionSystem.h"
#include "CollisionUtility.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"

namespace nc::physics
{
    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    CollisionSystem::CollisionSystem()
        : m_colliderSystem{config::Get().memory.maxColliders},
          m_dynamicEstimates{},
          m_broadEventsVsDynamic{},
          m_broadEventsVsStatic{},
          m_currentCollisions{},
          m_previousCollisions{}
    {
        internal::RegisterColliderSystem(&m_colliderSystem);
    }

    void CollisionSystem::DoCollisionStep()
    {
        FetchEstimates();
        BroadDetection();
        NarrowDetection();
        CompareToPreviousStep();
        Cleanup();
    }

    void CollisionSystem::ClearState()
    {
        m_colliderSystem.Clear();
        m_broadEventsVsDynamic.resize(0u);
        m_broadEventsVsStatic.resize(0u);
        m_currentCollisions.resize(0u);
        m_previousCollisions.resize(0u);
    }

    void CollisionSystem::FetchEstimates()
    {
        m_colliderSystem.GetDynamicSOA()->CalculateEstimates(&m_dynamicEstimates);
    }

    void CollisionSystem::BroadDetection()
    {
        /** possible optimization: Sphere vs Sphere in broad phase gives the actual result, not an estimate. 
         *  Can we detect this and add it right to m_currentStepEvents? */

        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);

        const auto dynamicSize = m_dynamicEstimates.size();
        auto* staticTree = m_colliderSystem.GetStaticTree();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            // Check vs other dynamic bodies
            for(size_t j = i + 1; j < dynamicSize; ++j)
            {
                if(m_dynamicEstimates[i].volumeEstimate.Intersects(m_dynamicEstimates[j].volumeEstimate))
                    m_broadEventsVsDynamic.emplace_back(m_dynamicEstimates[i].index, m_dynamicEstimates[j].index);
            }

            // Check vs bodies in static tree
            for(const auto* pair : staticTree->BroadCheck(m_dynamicEstimates[i].volumeEstimate))
            {
                // Because static volumes can exist in multiple tree nodes, identical intersections may be reported.
                const auto beg = m_broadEventsVsStatic.cbegin();
                const auto end = m_broadEventsVsStatic.cend();
                if(end == std::find_if(beg, end, [pair](const auto& event) { return event.second->handle == pair->handle; }))
                    m_broadEventsVsStatic.emplace_back(m_dynamicEstimates[i].index, pair);
            }
        }

        NC_PROFILE_END();
    }

    void CollisionSystem::NarrowDetection()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);

        /** If a dynamic collider has broad collision with another dynamic + static we calculate its volume
         *  twice here. How wacky does the code get if we compute it once? */

        auto* dynamicSoA = m_colliderSystem.GetDynamicSOA();
        const auto& handles = dynamicSoA->GetHandles();
        const auto& types = dynamicSoA->GetTypes();
        const auto& transforms = dynamicSoA->GetTransforms();
        const auto& centerExtentPairs = dynamicSoA->GetVolumeProperties();
        for(const auto& [i, j] : m_broadEventsVsDynamic)
        {
            const auto v1 = CalculateBoundingVolume(types[i], centerExtentPairs[i], transforms[i]);
            const auto v2 = CalculateBoundingVolume(types[j], centerExtentPairs[j], transforms[j]);
            if(std::visit([](auto&& a, auto&& b) { return a.Intersects(b); }, v1, v2))
                m_currentCollisions.emplace_back(handles[i], handles[j]);
        }

        for(auto& [dynamicIndex, staticPair] : m_broadEventsVsStatic)
        {
            const auto volume = CalculateBoundingVolume(types[dynamicIndex], centerExtentPairs[dynamicIndex], transforms[dynamicIndex]);
            if(std::visit([](auto&& a, auto&& b) { return a.Intersects(b); }, volume, staticPair->volume))
                m_currentCollisions.emplace_back(handles[dynamicIndex], static_cast<EntityHandle::Handle_t>(staticPair->handle));
        }

        NC_PROFILE_END();
    }
    
    void CollisionSystem::CompareToPreviousStep() const
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        auto currBeg = m_currentCollisions.cbegin();
        auto currEnd = m_currentCollisions.cend();
        for(const auto& prev : m_previousCollisions)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEventType::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEventType::Stay);
        }

        auto prevBeg = m_previousCollisions.cbegin();
        auto prevEnd = m_previousCollisions.cend();
        for(const auto& curr : m_currentCollisions)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEventType::Enter);
        }
        NC_PROFILE_END();
    }

    void CollisionSystem::NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const
    {
        auto e1 = GetEntity(EntityHandle{data.first});
        auto e2 = GetEntity(EntityHandle{data.second});
        switch(type)
        {
            case CollisionEventType::Enter:
            {
                if(e1) e1->SendOnCollisionEnter(e2);
                if(e2) e2->SendOnCollisionEnter(e1);
                break;
            }
            case CollisionEventType::Stay:
            {
                if(e1) e1->SendOnCollisionStay(e2);
                if(e2) e2->SendOnCollisionStay(e1);
                break;
            }
            case CollisionEventType::Exit:
            {
                if(e1) e1->SendOnCollisionExit(e2);
                if(e2) e2->SendOnCollisionExit(e1);
                break;
            }
            default:
                throw std::runtime_error("NotifyCollisionEvent - Unknown CollisionEventType");
        }
    }

    void CollisionSystem::Cleanup()
    {
        m_previousCollisions = std::move(m_currentCollisions);
        m_currentCollisions.clear();
        m_dynamicEstimates.clear();
        m_broadEventsVsDynamic.clear();
        m_broadEventsVsStatic.clear();
    }

    #ifdef NC_EDITOR_ENABLED
    void CollisionSystem::UpdateWidgets(graphics::FrameManager& frameManager)
    {
        for(auto& collider : m_colliderSystem.GetComponents())
        {
            collider->UpdateWidget(frameManager);
        }
    }
    #endif
} // namespace nc::phsyics