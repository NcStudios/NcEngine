#include "CollisionSystem.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"

#include <variant>

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

    using Bound_t = std::variant<DirectX::BoundingOrientedBox, DirectX::BoundingSphere>;

    void GetBound(const Collider* collider, Bound_t* bound)
    {
        static const auto unitBox = DirectX::BoundingOrientedBox{};
        static const auto unitSphere = DirectX::BoundingSphere{};

        switch(collider->GetType())
        {
            case ColliderType::Box:
            {
                *bound = unitBox;
                unitBox.Transform(std::get<DirectX::BoundingOrientedBox>(*bound), collider->GetTransformationMatrix());
                break;
            }
            case ColliderType::Sphere:
            {
                *bound = unitSphere;
                unitSphere.Transform(std::get<DirectX::BoundingSphere>(*bound), collider->GetTransformationMatrix());
                break;
            }
        }
    }

    void CollisionSystem::BuildCurrentFrameState(const std::vector<Collider*>& colliders)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);

        Bound_t iBound, jBound;

        const auto count = colliders.size();
        for(size_t i = 0u; i < count; ++i)
        {
            GetBound(colliders[i], &iBound);

            for(size_t j = i + 1; j < count; ++j)
            {
                GetBound(colliders[j], &jBound);

                if(std::visit([](auto&& i , auto&& j){ return i.Intersects(j); }, iBound, jBound))
                {
                    m_currentCollisions.emplace_back(colliders[i], colliders[j]);
                }
            }
        }


        // const auto count = colliders.size();
        // for(size_t i = 0u; i < count; ++i)
        // {
        //     switch(colliders[i]->GetType())
        //     {
        //         case ColliderType::Box:
        //         {
        //             unitBox.Transform(iBox, colliders[i]->GetTransformationMatrix());
        //             for(size_t j = i + 1; j < count; ++j)
        //             {
        //                 switch(colliders[j]->GetType())
        //                 {
        //                     case ColliderType::Box:
        //                     {
        //                         unitBox.Transform(jBox, colliders[j]->GetTransformationMatrix());
        //                         if(iBox.Intersects(jBox))
        //                             m_currentCollisions.emplace_back(colliders[i], colliders[j]);
        //                         break;
        //                     }
        //                     case ColliderType::Sphere:
        //                     {
        //                         unitSphere.Transform(jSphere, colliders[j]->GetTransformationMatrix());
        //                         if(iBox.Intersects(jSphere))
        //                             m_currentCollisions.emplace_back(colliders[i], colliders[j]);
        //                         break;
        //                     }
        //                 }
        //             }
        //             break;
        //         }
        //         case ColliderType::Sphere:
        //         {
        //             unitSphere.Transform(iSphere, colliders[i]->GetTransformationMatrix());
        //             for(size_t j = i + 1; j < count; ++j)
        //             {
        //                 switch(colliders[j]->GetType())
        //                 {
        //                     case ColliderType::Box:
        //                     {
        //                         unitBox.Transform(jBox, colliders[j]->GetTransformationMatrix());
        //                         if(iSphere.Intersects(jBox))
        //                             m_currentCollisions.emplace_back(colliders[i], colliders[j]);
        //                         break;
        //                     }
        //                     case ColliderType::Sphere:
        //                     {
        //                         unitSphere.Transform(jSphere, colliders[j]->GetTransformationMatrix());
        //                         if(iSphere.Intersects(jSphere))
        //                             m_currentCollisions.emplace_back(colliders[i], colliders[j]);
        //                         break;
        //                     }
        //                 }
        //             }
        //             break;
        //         }
        //     }
        // }
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
}