#pragma once

#include "Ecs.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"

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

    inline void CollisionSystem::DoCollisionStep(const std::vector<Collider*>& colliders)
    {
        BuildCurrentFrameState(colliders);
        CompareFrameStates();
        m_previousCollisions = std::move(m_currentCollisions);
        m_currentCollisions.clear();
    }

    inline void CollisionSystem::BuildCurrentFrameState(const std::vector<Collider*>& colliders)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        DirectX::BoundingOrientedBox unit, a, b;
        const auto count = colliders.size();
        for(size_t i = 0u; i < count; ++i)
        {
            unit.Transform(a, colliders[i]->GetTransformationMatrix());
            for(size_t j = i + 1; j < count; ++j)
            {
                unit.Transform(b, colliders[j]->GetTransformationMatrix());
                if(a.Intersects(b))
                    m_currentCollisions.emplace_back(colliders[i], colliders[j]);
            }
        }
        NC_PROFILE_END();
    }
    
    inline void CollisionSystem::CompareFrameStates() const
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        auto currBeg = m_currentCollisions.cbegin();
        auto currEnd = m_currentCollisions.cend();
        for(const auto& prev : m_previousCollisions)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEvent::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEvent::Stay);
        }

        auto prevBeg = m_previousCollisions.cbegin();
        auto prevEnd = m_previousCollisions.cend();
        for(const auto& curr : m_currentCollisions)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEvent::Enter);
        }
        NC_PROFILE_END();
    }

    inline void CollisionSystem::NotifyCollisionEvent(const CollisionData& data, CollisionEvent type) const
    {
        auto e1 = GetEntity(data.first->GetParentHandle());
        auto e2 = GetEntity(data.second->GetParentHandle());
        switch(type)
        {
            case CollisionEvent::Enter:
            {
                if(e1) e1->SendOnCollisionEnter(e2);
                if(e2) e2->SendOnCollisionEnter(e1);
                break;
            }
            case CollisionEvent::Stay:
            {
                if(e1) e1->SendOnCollisionStay(e2);
                if(e2) e2->SendOnCollisionStay(e1);
                break;
            }
            case CollisionEvent::Exit:
            {
                if(e1) e1->SendOnCollisionExit(e2);
                if(e2) e2->SendOnCollisionExit(e1);
                break;
            }
            default:
                throw std::runtime_error("NotifyCollisionEvent - Unknown CollisionEvent");
        }
    }

    inline void CollisionSystem::ClearState()
    {
        m_previousCollisions.clear();
    }

    inline bool operator ==(const CollisionData& lhs, const CollisionData& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }
}