#include "CollisionSystem.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"

#include <variant>

namespace
{
    using BoundingVolume = std::variant<DirectX::BoundingOrientedBox, DirectX::BoundingSphere>;

    template<class T>
    void TransformVolume(BoundingVolume& volume, const nc::Collider* collider)
    {
        static constexpr T unit{};
        volume = unit;
        unit.Transform(std::get<T>(volume), collider->GetTransformationMatrix());
    }

    void TransformVolume(BoundingVolume& volume, const nc::Collider* collider)
    {
        switch(collider->GetType())
        {
            case nc::ColliderType::Box:
            {
                TransformVolume<DirectX::BoundingOrientedBox>(volume, collider);
                break;
            }
            case nc::ColliderType::Sphere:
            {
                TransformVolume<DirectX::BoundingSphere>(volume, collider);
                break;
            }
            default:
            {
                throw std::runtime_error("GetBoundingVolume - Unknown ColliderType");
            }
        }
    }
} // anonymous namespace

namespace nc::physics
{
    bool operator ==(const CollisionData& lhs, const CollisionData& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    void CollisionSystem::DoCollisionStep(const std::vector<Collider*>& colliders)
    {
        BuildCurrentFrameState(colliders);
        CompareFrameStates();
        m_previousCollisions = std::move(m_currentCollisions);
        m_currentCollisions.clear();
    }

    void CollisionSystem::BuildCurrentFrameState(const std::vector<Collider*>& colliders)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
        BoundingVolume firstVolume, secondVolume;
        const auto count = colliders.size();
        for(size_t first = 0u; first < count; ++first)
        {
            TransformVolume(firstVolume, colliders[first]);
            for(size_t second = first + 1; second < count; ++second)
            {
                TransformVolume(secondVolume, colliders[second]);
                if(std::visit([](auto&& a , auto&& b){ return a.Intersects(b); }, firstVolume, secondVolume))
                {
                    m_currentCollisions.emplace_back(colliders[first], colliders[second]);
                }
            }
        }
        NC_PROFILE_END();
    }
    
    void CollisionSystem::CompareFrameStates() const
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

    void CollisionSystem::NotifyCollisionEvent(const CollisionData& data, CollisionEvent type) const
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

    void CollisionSystem::ClearState()
    {
        m_previousCollisions.clear();
    }
} // namespace nc::phsyics