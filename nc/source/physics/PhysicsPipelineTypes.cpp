#include "PhysicsPipelineTypes.h"
#include "debug/Utils.h"
#include "ecs/Registry.h"
#include "ecs/component/PhysicsBody.h"
#include "physics/PhysicsConstants.h"

#include <array>

namespace
{
    using namespace nc;

    constexpr float SquareContactBreakDistance = physics::ContactBreakDistance * physics::ContactBreakDistance;

    int ClosestAxis(float x, float y, float z, float w)
    {
        x = fabs(x); y = fabs(y); z = fabs(z); w = fabs(w);
        int maxIndex = 0;
        float maxVal = x;

        if(y > maxVal)
        {
            maxIndex = 1;
            maxVal = y;
        }

        if(z > maxVal)
        {
            maxIndex = 2;
            maxVal = z;
        }

        if(w > maxVal)
        {
            maxIndex = 3;
        }

        return maxIndex;
    }

    float CalcArea4Points(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        float mag0 = SquareMagnitude(CrossProduct(p0-p1, p2-p3));
        float mag1 = SquareMagnitude(CrossProduct(p0-p2, p1-p3));
        float mag2 = SquareMagnitude(CrossProduct(p0-p3, p1-p2));
        return math::Max(math::Max(mag0, mag1), mag2);
    }
}

namespace nc::physics
{
    auto GetColliderInteractionType(bool isTrigger, const PhysicsBody* body) -> ColliderInteractionType
    {
        // note: could probably store this in collider... have physics body send message in c'tor
        // to update as it already gets the collider pointer

        if(body)
        {
            if(body->IsKinematic())
            {
                if(isTrigger)
                {
                    return ColliderInteractionType::KinematicPhysicsTrigger;
                }

                return ColliderInteractionType::KinematicPhysics;
            }

            // need to add kinematic body
            if(isTrigger)
            {
                return ColliderInteractionType::PhysicsTrigger;
            }

            return ColliderInteractionType::Physics;
        }

        if(isTrigger)
        {
            return ColliderInteractionType::ColliderTrigger;
        }

        return ColliderInteractionType::Collider;
    }

    auto GetEventType(ColliderInteractionType typeA, ColliderInteractionType typeB) -> CollisionEventType
    {
        if(typeA == ColliderInteractionType::Collider)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::SecondBodyPhysics;
            
            if(typeB == ColliderInteractionType::PhysicsTrigger || typeB == ColliderInteractionType::KinematicPhysicsTrigger)
                return CollisionEventType::Trigger;
            
            return CollisionEventType::None;
        }

        if(typeA == ColliderInteractionType::Physics)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::TwoBodyPhysics;
            
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::KinematicPhysics)
                return CollisionEventType::FirstBodyPhysics;

            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::KinematicPhysics)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::SecondBodyPhysics;
            
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::KinematicPhysics)
                return CollisionEventType::None;
            
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::ColliderTrigger)
        {
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::ColliderTrigger)
                return CollisionEventType::None;
            
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::PhysicsTrigger)
        {
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::KinematicPhysicsTrigger)
        {
            return CollisionEventType::Trigger;
        }

        throw nc::NcError("Unknown ColliderInteractionType");
    }

    int Manifold::AddContact(const Contact& contact)
    {
        event.state = NarrowEvent::State::Persisting;
        int insertIndex = contacts.size();

        if(insertIndex >= 4)
        {
            insertIndex = SortPoints(contact);

            if(insertIndex < 0)
                insertIndex = 0;

            contacts[insertIndex] = contact;
        }
        else
        {
            contacts.push_back(contact);
        }

        return insertIndex;
    }

    const Contact& Manifold::GetDeepestContact() const
    {
        IF_THROW(contacts.size() == 0u, "Manifold::GetDeepestContact - Empty contacts");

        size_t maxPenetrationIndex = 0;
        float maxPenetration = contacts[0].depth;
        for(size_t i = 1u; i < contacts.size(); ++i)
        {
            if(contacts[i].depth > maxPenetration)
            {
                maxPenetrationIndex = i;
                maxPenetration = contacts[i].depth;
            }
        }

        return contacts[maxPenetrationIndex];
    }

    int Manifold::SortPoints(const Contact& contact)
    {
        int maxPenetrationIndex = -1;

        float maxPenetration = contact.depth;
        for(size_t i = 0; i < contacts.size(); ++i)
        {
            if(contacts[i].depth > maxPenetration)
            {
                maxPenetrationIndex = i;
                maxPenetration = contacts[i].depth;
            }
        }

        float res0 = 0.0f;
        float res1 = 0.0f;
        float res2 = 0.0f;
        float res3 = 0.0f;
        
        if(maxPenetrationIndex != 0)
            res0 = CalcArea4Points(contact.localPointA, contacts[1].localPointA, contacts[2].localPointA, contacts[3].localPointA);

        if(maxPenetrationIndex != 1)
            res1 = CalcArea4Points(contact.localPointA, contacts[0].localPointA, contacts[2].localPointA, contacts[3].localPointA);

        if(maxPenetrationIndex != 2)
            res2 = CalcArea4Points(contact.localPointA, contacts[0].localPointA, contacts[1].localPointA, contacts[3].localPointA);

        if(maxPenetrationIndex != 3)
            res3 = CalcArea4Points(contact.localPointA, contacts[0].localPointA, contacts[1].localPointA, contacts[2].localPointA);

        int biggestArea = ClosestAxis(res0, res1, res2, res3);
        return biggestArea;
    }

    void Manifold::UpdateWorldPoints(const Registry* registry)
    {
        /** @todo Manifolds can linger after objects are destroyed. The check below prevents problems,
         *  but the caller then has to destroy this manifold upon detecting it is empty. It would be
         *  cleaner to handle this through registy callbacks, but we can't because the BspTree uses the
         *  ConcaveCollider callback. Long story short, I think we need to support multiple callbacks per
         *  component type. */
        const auto* transformA = registry->Get<Transform>(event.first);
        const auto* transformB = registry->Get<Transform>(event.second);
        if(!transformA || !transformB)
        {
            contacts.clear();
            return;
        }

        const auto& aMatrix = transformA->GetTransformationMatrix();
        const auto& bMatrix = transformB->GetTransformationMatrix();

        for(auto cur = contacts.rbegin(); cur != contacts.rend(); ++cur)
        {
            auto& contact = *cur;

            auto pointA_v = DirectX::XMLoadVector3(&contact.localPointA);
            auto pointB_v = DirectX::XMLoadVector3(&contact.localPointB);

            pointA_v = DirectX::XMVector3Transform(pointA_v, aMatrix);
            pointB_v = DirectX::XMVector3Transform(pointB_v, bMatrix);

            DirectX::XMStoreVector3(&contact.worldPointA, pointA_v);
            DirectX::XMStoreVector3(&contact.worldPointB, pointB_v);

            contact.depth = Dot(contact.worldPointA - contact.worldPointB, contact.normal);

            if(contact.depth < ContactBreakDistance)
            {
                *cur = contacts.back();
                contacts.pop_back();
                continue;
            }

            auto projectedPoint = contact.worldPointA - contact.normal * contact.depth;
            auto projectedDifference = contact.worldPointB - projectedPoint;
            auto distance2d = SquareMagnitude(projectedDifference);
            if(distance2d > SquareContactBreakDistance)
            {
                *cur = contacts.back();
                contacts.pop_back();
            }
        }
    }
}