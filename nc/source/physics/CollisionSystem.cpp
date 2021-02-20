#include "CollisionSystem.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"

namespace nc::physics
{
    bool operator ==(const CollisionEvent& lhs, const CollisionEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    CollisionSystem::CollisionSystem()
        : m_colliderSystem{},
          m_dynamicColliders{},
          m_staticColliders{},
          m_broadPhaseEvents{},
          m_currentStepEvents{},
          m_previousStepEvents{}
    {
        internal::RegisterColliderSystem(&m_colliderSystem);
    }

    void CollisionSystem::DoCollisionStep()
    {
        FetchColliders();
        BroadPhase();
        NarrowPhase();
        CompareFrameStates();
        Cleanup();
    }

    void CollisionSystem::ClearState()
    {
        m_colliderSystem.ClearState();
        m_dynamicColliders.resize(0u);
        m_staticColliders.resize(0u);
        m_broadPhaseEvents.resize(0u);
        m_currentStepEvents.resize(0u);
        m_previousStepEvents.resize(0u);
    }

    void CollisionSystem::FetchColliders()
    {
        m_colliderSystem.GetDynamicSystem()->ForEach([this](auto& collider)
        {
            m_dynamicColliders.push_back(&collider);
        });

        /** @todo static boys should be in a tree, but where do we plant it? */
        m_colliderSystem.GetStaticSystem()->ForEach([this](auto& collider)
        {
            m_staticColliders.push_back(&collider);
        });
    }

    void CollisionSystem::BroadPhase()
    {
        /** possible optimization: Sphere vs Sphere in broad phase gives the actual result, not an estimate. 
         *  Can we detect this and add it right to m_currentStepEvents? */

        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        DirectX::BoundingSphere firstEst;
        const auto dynamicCount = m_dynamicColliders.size();
        const auto staticCount = m_staticColliders.size();
        for(size_t first = 0u; first < dynamicCount; ++first)
        {
            firstEst = m_dynamicColliders[first]->EstimateBoundingVolume();
            for(size_t second = first + 1; second < dynamicCount; ++second)
            {
                if(firstEst.Intersects(m_dynamicColliders[second]->EstimateBoundingVolume()))
                {
                    m_broadPhaseEvents.emplace_back(m_dynamicColliders[first], m_dynamicColliders[second]);
                }
            }

            /** Temporary check against statics - with a can-do attitude this can be done in log n time. */
            for(size_t second = 0u; second < staticCount; ++second)
            {
                if(firstEst.Intersects(m_staticColliders[second]->EstimateBoundingVolume()))
                {
                    m_broadPhaseEvents.emplace_back(m_dynamicColliders[first], m_staticColliders[second]);
                }
            }
        }
        NC_PROFILE_END();
    }

    void CollisionSystem::NarrowPhase()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        for(const auto& event : m_broadPhaseEvents)
        {
            if(std::visit([](auto&& a, auto&& b){ return a.Intersects(b); }, event.first->CalculateBoundingVolume(), event.second->CalculateBoundingVolume()))
            {
                m_currentStepEvents.emplace_back(event.first, event.second);
            }
        }
        NC_PROFILE_END();
    }
    
    void CollisionSystem::CompareFrameStates() const
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        auto currBeg = m_currentStepEvents.cbegin();
        auto currEnd = m_currentStepEvents.cend();
        for(const auto& prev : m_previousStepEvents)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEventType::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEventType::Stay);
        }

        auto prevBeg = m_previousStepEvents.cbegin();
        auto prevEnd = m_previousStepEvents.cend();
        for(const auto& curr : m_currentStepEvents)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEventType::Enter);
        }
        NC_PROFILE_END();
    }

    void CollisionSystem::NotifyCollisionEvent(const CollisionEvent& data, CollisionEventType type) const
    {
        auto e1 = GetEntity(data.first->GetParentHandle());
        auto e2 = GetEntity(data.second->GetParentHandle());
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
        m_previousStepEvents = std::move(m_currentStepEvents);
        m_dynamicColliders.clear();
        m_staticColliders.clear();
        m_currentStepEvents.clear();
        m_broadPhaseEvents.clear();
    }

    #ifdef NC_EDITOR_ENABLED
    void CollisionSystem::UpdateWidgets(graphics::FrameManager& frameManager)
    {
        m_colliderSystem.GetDynamicSystem()->ForEach([&frameManager](auto& collider)
        {
            collider.UpdateWidget(frameManager);
        });
        
        m_colliderSystem.GetStaticSystem()->ForEach([&frameManager](auto& collider)
        {
            collider.UpdateWidget(frameManager);
        });
    }
    #endif
} // namespace nc::phsyics