#pragma once

#include "Ecs.h"
#include "directx/math/DirectXCollision.h"
#include "debug/Profiler.h"

#include <cstdint>
#include <vector>

namespace nc::physics::collision
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

    namespace
    {
        std::vector<CollisionData> previousCollisions{}; // temp until state management is decided
    }

    inline void DoCollisionStep(const std::vector<Collider*>& colliders);
    inline void BuildCurrentFrameState(const std::vector<Collider*>& colliders, std::vector<CollisionData>& out);
    inline void CompareFrameStates(const std::vector<CollisionData>& previousState, const std::vector<CollisionData>& currentState);
    inline void NotifyCollisionEvent(const CollisionData& data, CollisionEvent type);
    inline void ClearState();
    inline bool operator ==(const CollisionData& lhs, const CollisionData& rhs);

    inline void DoCollisionStep(const std::vector<Collider*>& colliders)
    {
        std::vector<CollisionData> currentCollisions;
        currentCollisions.reserve(previousCollisions.size());
        BuildCurrentFrameState(colliders, currentCollisions);
        CompareFrameStates(previousCollisions, currentCollisions);
        previousCollisions = std::move(currentCollisions);
    }

    inline void BuildCurrentFrameState(const std::vector<Collider*>& colliders, std::vector<CollisionData>& out)
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
                    out.emplace_back(colliders[i], colliders[j]);
            }
        }
        NC_PROFILE_END();
    }
    
    inline void CompareFrameStates(const std::vector<CollisionData>& previous, const std::vector<CollisionData>& current)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        auto currBeg = current.cbegin();
        auto currEnd = current.cend();
        for(auto& prev : previous)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEvent::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEvent::Stay);
        }

        auto prevBeg = previous.cbegin();
        auto prevEnd = previous.cend();
        for(auto& curr : current)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEvent::Enter);
        }
        NC_PROFILE_END();
    }

    inline void NotifyCollisionEvent(const CollisionData& data, CollisionEvent type)
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

    inline void ClearState()
    {
        previousCollisions.clear();
    }

    inline bool operator ==(const CollisionData& lhs, const CollisionData& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }
}