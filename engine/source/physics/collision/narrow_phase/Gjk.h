#pragma once

#include "Epa.h"

namespace nc::physics
{
    constexpr size_t GjkMaxIterations = 10u;

    template<bool GenerateContacts, class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* stateOut);

    template<class BVA>
    bool GjkVsTriangle(const BVA& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);

    template<class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b);

    bool RefinePoint(Simplex& simplex, Vector3& direction);
    bool RefineLine(Simplex& simplex, Vector3& direction);
    bool RefineTriangle(Simplex& simplex, Vector3& direction);
    bool RefineTetrahedron(Simplex& simplex, Vector3& direction);

    constexpr std::array<bool(*)(Simplex&, Vector3&), 4u> RefineSimplex = 
    {
        RefinePoint, RefineLine, RefineTriangle, RefineTetrahedron
    };

    bool RefinePoint(Simplex& simplex, Vector3& direction)
    {
        direction = -simplex[0];
        return false;
    }

    bool RefineLine(Simplex& simplex, Vector3& direction)
    {
        auto ao = -1.0f * simplex[0];
        auto ab = simplex[1] + ao;

        if(SameDirection(ab, ao))
        {
            direction = TripleCrossProduct(ab, ao, ab);
        }
        else
        {
            simplex.ToPoint<0u>();
            direction = ao;
        }

        return false;
    }

    bool RefineTriangle(Simplex& simplex, Vector3& direction)
    {
        auto ao = -1.0f * simplex[0];
        auto ab = simplex[1] + ao;
        auto ac = simplex[2] + ao;
        auto abc = CrossProduct(ab, ac);

        if(SameDirection(CrossProduct(abc, ac), ao))
        {
            if(SameDirection(ac, ao))
            {
                simplex.ToLine<0u, 2u>();
                direction = TripleCrossProduct(ac, ao, ac);
            }
            else
            {
                simplex.ToLine<0u, 1u>();
                return RefineLine(simplex, direction);
            }
        }
        else
        {
            if(SameDirection(CrossProduct(ab, abc), ao))
            {
                simplex.ToLine<0u, 1u>();
                return RefineLine(simplex, direction);
            }
            else
            {
                if(SameDirection(abc, ao))
                {
                    direction = abc;
                }
                else
                {
                    simplex.ToTriangle<0u, 2u, 1u>();
                    direction = -1.0f * abc;
                }
            }
        }

        return false;
    }

    bool RefineTetrahedron(Simplex& simplex, Vector3& direction)
    {
        auto ao = -1.0f * simplex[0];
        auto ab = simplex[1] + ao;
        auto ac = simplex[2] + ao;
        auto ad = simplex[3] + ao;
        auto abc = CrossProduct(ab, ac);
        auto acd = CrossProduct(ac, ad);
        auto adb = CrossProduct(ad, ab);

        if(SameDirection(abc, ao))
        {
            simplex.ToTriangle<0u, 1u, 2u>();
            return RefineTriangle(simplex, direction);
        }
        
        if(SameDirection(acd, ao))
        {
            simplex.ToTriangle<0u, 2u, 3u>();
            return RefineTriangle(simplex, direction);
        }

        if(SameDirection(adb, ao))
        {
            simplex.ToTriangle<0u, 3u, 1u>();
            return RefineTriangle(simplex, direction);
        }

        return true;
    }

    template<bool GenerateContacts, class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        using namespace DirectX;

        NC_ASSERT(stateOut, "CollisionState cannot be null");

        stateOut->simplex = Simplex{};
        stateOut->rotationA = GetRotation(aMatrix);
        stateOut->rotationB = GetRotation(bMatrix);
        Vector3 direction = Vector3::One();
        size_t itCount = 0;

        while(++itCount <= GjkMaxIterations)
        {
            const auto direction_v = XMLoadVector3(&direction);
            auto supports = MinkowskiSupports(a, b, stateOut->rotationA, stateOut->rotationB, aMatrix, bMatrix, direction_v);

            if(XMVector3Less(XMVector3Dot(supports.worldCSO, direction_v), g_XMZero))
                break;

            Vector3 supportCSO, worldSupportA, worldSupportB, localSupportA, localSupportB;
            XMStoreVector3(&supportCSO, supports.worldCSO);
            XMStoreVector3(&worldSupportA, supports.worldA);
            XMStoreVector3(&worldSupportB, supports.worldB);
            XMStoreVector3(&localSupportA, supports.localA);
            XMStoreVector3(&localSupportB, supports.localB);

            stateOut->simplex.PushFront(supportCSO, worldSupportA, worldSupportB, localSupportA, localSupportB);

            if(RefineSimplex[stateOut->simplex.Size() - 1](stateOut->simplex, direction))
            {
                /** In the case where the origin is on or very near a simplex feature we can end up adding
                 *  the same vertex multiple times. This solution works, but I'm pretty sure we could catch
                 *  it earlier. Also, this is only a problem for contact generation. I don't see anything 
                 *  wrong with returning true for trigger cases, if we wanted.
                 *  @todo In fact, we might fail to detect 'perfectly centered' triggers if we don't return
                 *  true here. Should be verified. */
                if(!stateOut->simplex.HasAllUniqueVertices())
                    return false;

                if constexpr(GenerateContacts)
                    return Epa<BVA, BVB>(a, b, aMatrix, bMatrix, stateOut);
                else
                    return true;
            }
        }

        return false;
    }

    template<class BVA>
    bool GjkVsTriangle(const BVA& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut)
    {
        using namespace DirectX;

        stateOut->simplex = Simplex{};
        stateOut->rotationA = GetRotation(aMatrix);
        Vector3 direction = Vector3::One();
        size_t itCount = 0;

        auto bXM = TriangleXM
        {
            .a = XMLoadVector3(&b.a),
            .b = XMLoadVector3(&b.b),
            .c = XMLoadVector3(&b.c)
        };

        while(++itCount <= GjkMaxIterations)
        {
            const auto direction_v = XMLoadVector3(&direction);
            auto aDirection_v = XMVector3InverseRotate(direction_v, stateOut->rotationA);
            auto aSupportLocal_v = MinkowskiSupport(a, aDirection_v);
            auto aSupportWorld_v = XMVector3Transform(aSupportLocal_v, aMatrix);

            auto bDirection_v = XMVectorNegate(direction_v);
            auto bSupportWorld_v = MinkowskiSupport(bXM, bDirection_v);
            auto supportCSO_v = XMVectorSubtract(aSupportWorld_v, bSupportWorld_v);

            if(XMVector3Less(XMVector3Dot(supportCSO_v, direction_v), g_XMZero))
                break;

            Vector3 supportCSO, worldSupportA, worldSupportB, localSupportA;
            XMStoreVector3(&supportCSO, supportCSO_v);
            XMStoreVector3(&worldSupportA, aSupportWorld_v);
            XMStoreVector3(&worldSupportB, bSupportWorld_v);
            XMStoreVector3(&localSupportA, aSupportLocal_v);

            stateOut->simplex.PushFront(supportCSO, worldSupportA, worldSupportB, localSupportA, worldSupportB);

            if(RefineSimplex[stateOut->simplex.Size() - 1](stateOut->simplex, direction))
            {
                if(!stateOut->simplex.HasAllUniqueVertices())
                    return false;

                return EpaVsTriangle(a, bXM, aMatrix, stateOut);
            }
        }

        return false;
    }

    template<class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b)
    {
        using namespace DirectX;
        
        Vector3 direction = Vector3::One();
        Simplex simplex;
        size_t itCount = 0;

        while(++itCount <= GjkMaxIterations)
        {
            const auto direction_v = XMLoadVector3(&direction);
            auto supportA_v = MinkowskiSupport(a, direction_v);
            auto supportB_v = MinkowskiSupport(b, XMVectorNegate(direction_v));
            auto supportCSO_v =  XMVectorSubtract(supportA_v, supportB_v);

            if(XMVector3Less(XMVector3Dot(supportCSO_v, direction_v), g_XMZero))
                break;

            Vector3 supportCSO, supportA, supportB;
            XMStoreVector3(&supportCSO, supportCSO_v);
            XMStoreVector3(&supportA, supportA_v);
            XMStoreVector3(&supportB, supportB_v);

            simplex.PushFront(supportCSO, supportA, supportB, supportA, supportB);

            if(RefineSimplex[simplex.Size() - 1](simplex, direction))
            {
                return simplex.HasAllUniqueVertices();
            }
        }

        return false;
    }
}