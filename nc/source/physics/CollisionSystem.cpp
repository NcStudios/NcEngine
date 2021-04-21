#include "CollisionSystem.h"
#include "CollisionUtility.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"
#include "job/JobSystem.h"

namespace nc::physics
{
    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    CollisionSystem::CollisionSystem(uint32_t maxDynamicColliders,
                                     uint32_t maxStaticColliders,
                                     uint32_t octreeDensityThreshold,
                                     float octreeMinimumExtent,
                                     float worldspaceExtent,
                                     job::JobSystem* jobSystem)
        : m_colliderSystem{maxDynamicColliders, maxStaticColliders, octreeDensityThreshold, octreeMinimumExtent, worldspaceExtent},
          m_dynamicEstimates{},
          m_broadEventsVsDynamic{},
          m_broadEventsVsStatic{},
          m_currentCollisions{},
          m_previousCollisions{},
          m_jobSystem{jobSystem}
    {
        internal::RegisterColliderSystem(&m_colliderSystem);
    }

    ecs::ComponentSystem<Collider>* CollisionSystem::GetColliderSystem()
    {
        return m_colliderSystem.GetComponentSystem();
    }

    void CollisionSystem::DoCollisionStep()
    {
        FetchEstimates();

        auto broadDynamicJobResult = m_jobSystem->Schedule(BroadDetectVsDynamic, this);
        auto broadStaticJobResult = m_jobSystem->Schedule(BroadDetectVsStatic, this);
        broadDynamicJobResult.wait();
        broadStaticJobResult.wait();
        
        auto narrowDynamicJobResult = m_jobSystem->Schedule(NarrowDetectVsDynamic, this);
        auto narrowStaticJobResult = m_jobSystem->Schedule(NarrowDetectVsStatic, this);
        narrowDynamicJobResult.wait();
        broadStaticJobResult.wait();

        auto findEnterAndStayJobResult = m_jobSystem->Schedule(FindEnterAndStayEvents, this);
        auto findExitJobResult = m_jobSystem->Schedule(FindExitEvents, this);
        findEnterAndStayJobResult.wait();
        findExitJobResult.wait();
        
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
        const auto* soa = m_colliderSystem.GetDynamicSoA();
        const auto properties = soa->GetSpan<VolumeProperties>();
        const auto transforms = soa->GetSpan<const DirectX::XMMATRIX*>();
        auto index = soa->SmartIndex();
        while(index.Valid())
        {
            m_dynamicEstimates.emplace_back(EstimateBoundingVolume(properties[index], transforms[index]), index);
            ++index;
        }
    }

    void CollisionSystem::BroadDetectVsDynamic()
    {
        const auto dynamicSize = m_dynamicEstimates.size();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            // Check vs other dynamic bodies
            for(size_t j = i + 1; j < dynamicSize; ++j)
            {
                if(m_dynamicEstimates[i].volumeEstimate.Intersects(m_dynamicEstimates[j].volumeEstimate))
                    m_broadEventsVsDynamic.emplace_back(m_dynamicEstimates[i].index, m_dynamicEstimates[j].index);
            }
        }
    }

    void CollisionSystem::BroadDetectVsStatic()
    {
        const auto dynamicSize = m_dynamicEstimates.size();
        auto* staticTree = m_colliderSystem.GetStaticTree();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
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
    }

    /** @todo Narrow detection computes the bounding volumes for each pair encountered,
     *  potentially calculating the same volume multiple times. This could be done once
     *  upfront. */
    void CollisionSystem::NarrowDetectVsDynamic()
    {
        auto* dynamicSoA = m_colliderSystem.GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityHandle::Handle_t>();
        const auto types = dynamicSoA->GetSpan<ColliderType>();
        const auto transforms = dynamicSoA->GetSpan<const DirectX::XMMATRIX*>();
        const auto properties = dynamicSoA->GetSpan<VolumeProperties>();
        for(const auto& [i, j] : m_broadEventsVsDynamic)
        {
            const auto v1 = CalculateBoundingVolume(types[i], properties[i], transforms[i]);
            const auto v2 = CalculateBoundingVolume(types[j], properties[j], transforms[j]);
            if(std::visit([](auto&& a, auto&& b) { return a.Intersects(b); }, v1, v2))
                m_currentCollisions.emplace_back(handles[i], handles[j]);
        }
    }

    void CollisionSystem::NarrowDetectVsStatic()
    {
        auto* dynamicSoA = m_colliderSystem.GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityHandle::Handle_t>();
        const auto types = dynamicSoA->GetSpan<ColliderType>();
        const auto transforms = dynamicSoA->GetSpan<const DirectX::XMMATRIX*>();
        const auto properties = dynamicSoA->GetSpan<VolumeProperties>();
        for(auto& [dynamicIndex, staticPair] : m_broadEventsVsStatic)
        {
            const auto volume = CalculateBoundingVolume(types[dynamicIndex], properties[dynamicIndex], transforms[dynamicIndex]);
            if(std::visit([](auto&& a, auto&& b) { return a.Intersects(b); }, volume, staticPair->volume))
                m_currentCollisions.emplace_back(handles[dynamicIndex], static_cast<EntityHandle::Handle_t>(staticPair->handle));
        }
    }
    
    void CollisionSystem::FindEnterAndStayEvents() const
    {
        auto currBeg = m_currentCollisions.cbegin();
        auto currEnd = m_currentCollisions.cend();
        for(const auto& prev : m_previousCollisions)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEventType::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEventType::Stay);
        }
    }

    void CollisionSystem::FindExitEvents() const
    {
        auto prevBeg = m_previousCollisions.cbegin();
        auto prevEnd = m_previousCollisions.cend();
        for(const auto& curr : m_currentCollisions)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEventType::Enter);
        }
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
    void CollisionSystem::UpdateWidgets(graphics::FrameManager* frameManager)
    {
        for(auto& collider : m_colliderSystem.GetComponents())
        {
            collider->UpdateWidget(frameManager);
        }
    }
    #endif
} // namespace nc::phsyics