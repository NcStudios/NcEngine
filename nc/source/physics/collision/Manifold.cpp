#include "Manifold.h"
#include "physics/PhysicsConstants.h"

#include <array>

namespace nc::physics
{
    bool operator == (const Contact& a, const Contact& b)
    {
        return (a.normal == b.normal) &&
               (
                   (a.worldPointA == b.worldPointA && a.worldPointB == b.worldPointB) ||
                   (a.worldPointA == b.worldPointB && a.worldPointB == b.worldPointA)
               );
    }

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

    int Manifold::AddContact(const Contact& contact)
    {
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

    void Manifold::UpdateWorldPoints(registry_type* registry)
    {
        const auto& aMatrix = registry->Get<Transform>(Entity{entityA})->GetTransformationMatrix();
        const auto& bMatrix = registry->Get<Transform>(Entity{entityB})->GetTransformationMatrix();

        for(auto cur = contacts.rbegin(); cur != contacts.rend(); ++cur)
        {
            auto& contact = *cur;

            auto pointA_v = DirectX::XMLoadVector3(&contact.localPointA);
            auto pointB_v = DirectX::XMLoadVector3(&contact.localPointB);

            pointA_v = DirectX::XMVector3Transform(pointA_v, aMatrix);
            pointB_v = DirectX::XMVector3Transform(pointB_v, bMatrix);

            DirectX::XMStoreVector3(&contact.worldPointA, pointA_v);
            DirectX::XMStoreVector3(&contact.worldPointB, pointB_v);

            float newDepth = Dot(contact.worldPointA - contact.worldPointB, contact.normal);
            contact.depth = newDepth;

            if(newDepth < ContactBreakDistance)
            {
                *cur = contacts.back();
                contacts.pop_back();
                continue;
            }

            auto projectedPoint = contact.worldPointA - contact.normal * contact.depth;
            auto projectedDifference = contact.worldPointB - projectedPoint;
            auto distance2d = Dot(projectedDifference, projectedDifference);
            if(distance2d > ContactBreakDistance * ContactBreakDistance)
            {
                *cur = contacts.back();
                contacts.pop_back();
            }
        }
    }
}