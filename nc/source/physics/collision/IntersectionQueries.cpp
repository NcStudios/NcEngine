#include "IntersectionQueries.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"

#include <algorithm>
#include <array>
#include <stdexcept>

#include <iostream> // remove once minDistance error is figured out

/** @todo 
 *  - Rather than having MinkowskiSupport determine the collider type,
 *    figure it out at the top level call. This will branch once per collider
 *    rather than each gjk/epa loop iteration.
 *  - Collisions can be more efficient in certain situations. For example, 
 *    narrow phase sphere vs. sphere uses full gjk + epa, which is silly. */

namespace nc::physics
{
    using namespace DirectX;

    constexpr float FloatMax = std::numeric_limits<float>::max();
    constexpr float EpaTolerance = 0.001f;
    constexpr size_t GjkMaxIterations = 10u;
    constexpr size_t EpaMaxIterations = 32u;

    bool SameDirection(const Vector3& a, const Vector3& b);
    bool RefinePoint(Simplex& simplex, Vector3& direction);
    bool RefineLine(Simplex& simplex, Vector3& direction);
    bool RefineTriangle(Simplex& simplex, Vector3& direction);
    bool RefineTetrahedron(Simplex& simplex, Vector3& direction);
    XMVECTOR GetRotation(FXMMATRIX matrix);
    XMVECTOR MinkowskiSupport(const BoundingVolume& collider, DirectX::FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const SphereCollider& collider, DirectX::FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const BoxCollider& collider, DirectX::FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const CapsuleCollider& collider, DirectX::FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const HullCollider& collider, DirectX::FXMVECTOR direction_v);
    
    constexpr std::array<bool(*)(Simplex&, Vector3&), 4u> RefineSimplex = 
    {
        RefinePoint, RefineLine, RefineTriangle, RefineTetrahedron
    };

    bool Intersect(const BoundingVolume& a, const BoundingVolume& b)
    {
        Vector3 direction = Vector3::One();
        Simplex simplex;
        size_t itCount = 0;

        while(++itCount <= GjkMaxIterations)
        {
            const auto direction_v = XMLoadVector3(&direction);
            auto supportA_v = MinkowskiSupport(a, direction_v);
            auto supportB_v = MinkowskiSupport(b, XMVectorNegate(direction_v));
            auto supportCSO_v =  supportA_v - supportB_v;

            if(XMVector3Less(XMVector3Dot(supportCSO_v, direction_v), g_XMZero))
                break;

            Vector3 supportCSO, supportA, supportB;
            XMStoreVector3(&supportCSO, supportCSO_v);
            XMStoreVector3(&supportA, supportA_v);
            XMStoreVector3(&supportB, supportB_v);

            simplex.PushFront(supportCSO, supportA, supportB, supportA, supportB);

            if(RefineSimplex[simplex.Size() - 1](simplex, direction))
                return true;
        }

        return false;
    }

    bool Intersect(const SphereCollider& a, const SphereCollider& b)
    {
        auto radii = a.radius + b.radius;
        return SquareMagnitude(a.center - b.center) < radii * radii;
    }

    bool Intersect(const SphereCollider& sphere, const BoxCollider& aabb)
    {
        auto squareDistance = SquareMtdToAABB(sphere.center, aabb);
        return squareDistance <= sphere.radius * sphere.radius;
    }

    bool Gjk(const BoundingVolume& a, const BoundingVolume& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        IF_THROW(!stateOut, "Gjk - CollisionState cannot be null");

        stateOut->simplex = Simplex{};
        stateOut->rotationA = GetRotation(aMatrix);
        stateOut->rotationB = GetRotation(bMatrix);
        Vector3 direction = Vector3::One();
        size_t itCount = 0;

        while(++itCount <= GjkMaxIterations)
        {
            const auto direction_v = XMLoadVector3(&direction);
            auto aDirection_v = XMVector3InverseRotate(direction_v, stateOut->rotationA);
            auto aSupportLocal_v = MinkowskiSupport(a, aDirection_v);
            auto aSupportWorld_v = XMVector3Transform(aSupportLocal_v, aMatrix);
            auto bDirection_v = XMVector3InverseRotate(XMVectorNegate(direction_v), stateOut->rotationB);
            auto bSupportLocal_v = MinkowskiSupport(b, bDirection_v);
            auto bSupportWorld_v = XMVector3Transform(bSupportLocal_v, bMatrix);
            auto supportCSO_v = aSupportWorld_v - bSupportWorld_v;

            if(XMVector3Less(XMVector3Dot(supportCSO_v, direction_v), g_XMZero))
                break;

            Vector3 supportCSO, worldSupportA, worldSupportB, localSupportA, localSupportB;
            XMStoreVector3(&supportCSO, supportCSO_v);
            XMStoreVector3(&worldSupportA, aSupportWorld_v);
            XMStoreVector3(&worldSupportB, bSupportWorld_v);
            XMStoreVector3(&localSupportA, aSupportLocal_v);
            XMStoreVector3(&localSupportB, bSupportLocal_v);

            stateOut->simplex.PushFront(supportCSO, worldSupportA, worldSupportB, localSupportA, localSupportB);

            if(RefineSimplex[stateOut->simplex.Size() - 1](stateOut->simplex, direction))
            {
                return true;
            }
        }

        return false;
    }

    bool Epa(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state)
    {
        /** @todo Storing the points for contact could be cleaner/more efficient */

        state->polytope.Initialize(state->simplex);
        auto minFace = state->polytope.ComputeNormalData();
        NormalData minNorm{Vector3::Zero(), FloatMax};
        size_t iterations = 0u;

        while(minNorm.distance == FloatMax)
        {
            // Normal data for the face closest to the origin.
            minNorm = state->polytope.GetNormalData(minFace);

            if(iterations++ > EpaMaxIterations)
                break;

            // Find a point on the Minkowski hull in the direction of the closest face's normal.
            auto direction_v = XMLoadVector3(&minNorm.normal);
            auto aDirection_v = XMVector3InverseRotate(direction_v, state->rotationA);
            auto aSupportLocal_v = MinkowskiSupport(a, aDirection_v);
            auto aSupportWorld_v = XMVector3Transform(aSupportLocal_v, aMatrix);
            auto bDirection_v = XMVector3InverseRotate(XMVectorNegate(direction_v), state->rotationB);
            auto bSupportLocal_v = MinkowskiSupport(b, bDirection_v);
            auto bSupportWorld_v = XMVector3Transform(bSupportLocal_v, bMatrix);
            auto support_v = aSupportWorld_v - bSupportWorld_v;
            Vector3 support;
            XMStoreVector3(&support, support_v);

            XMStoreVector3(&(state->contact.worldPointA), aSupportWorld_v);
            XMStoreVector3(&(state->contact.worldPointB), bSupportWorld_v);
            XMStoreVector3(&(state->contact.localPointA), aSupportLocal_v);
            XMStoreVector3(&(state->contact.localPointB), bSupportLocal_v);

            if(abs(Dot(minNorm.normal, support) - minNorm.distance) > EpaTolerance)
            {
                // The closest face is not on the hull, so we expand towards the hull.
                if(!state->polytope.Expand(support, state->contact, &minFace))
                {
                    /** @todo Need to determine if this can happen under normal circumstances.
                     *  It has thrown here a few times, but always when there is wonk elsewhere. */
                    //throw std::runtime_error("Epa - minDistance not found");
                    std::cout << "Epa - minDistance not found\n";
                    state->contact.depth = 0.0f;
                    return false;
                }

                minNorm.distance = FloatMax;
            }
        }

        auto success = state->polytope.GetContacts(minFace, &state->contact);
        state->contact.normal = Normalize(minNorm.normal);
        state->contact.depth = minNorm.distance + EpaTolerance;
        return success;
    }

    XMVECTOR GetRotation(FXMMATRIX matrix)
    {
        XMVECTOR scl, rot, pos;
        XMMatrixDecompose(&scl, &rot, &pos, matrix);
        return rot;
    }

    bool SameDirection(const Vector3& a, const Vector3& b)
    {
        return Dot(a, b) > 0.0f;
    }

    float SquareMtdToAABB(const Vector3& point, const BoxCollider& aabb)
    {
        auto SingleAxisDistance = [](float point, float min, float max)
        {
            float out = 0.0f;
            if(point < min)
                out += (min - point) * (min - point);
            else if(point > max)
                out += (point - max) * (point - max);
            return out;
        };

        auto halfExtents = aabb.extents / 2.0f;
        auto min = aabb.center - halfExtents;
        auto max = aabb.center + halfExtents;
        return SingleAxisDistance(point.x, min.x, max.x) +
               SingleAxisDistance(point.y, min.y, max.y) +
               SingleAxisDistance(point.z, min.z, max.z);
    }

    XMVECTOR MinkowskiSupport(const SphereCollider& collider, FXMVECTOR direction_v)
    {
        return XMLoadVector3(&collider.center) +
               XMVectorScale(XMVector3Normalize(direction_v), collider.radius);
    }

    XMVECTOR MinkowskiSupport(const BoxCollider& collider, FXMVECTOR direction_v)
    {
        Vector3 dir;
        XMStoreVector3(&dir, direction_v);

        auto sign = [](float n) { return n < 0.0f ? -1.0f : 1.0f; };

        auto extent = Vector3
        {
            sign(dir.x) * collider.extents.x / 2.0f,
            sign(dir.y) * collider.extents.y / 2.0f,
            sign(dir.z) * collider.extents.z / 2.0f
        };

        dir = collider.center + extent;
        return XMLoadVector3(&dir);
    }

    XMVECTOR MinkowskiSupport(const CapsuleCollider& collider, FXMVECTOR direction_v)
    {
        Vector3 normalizedDirection;
        XMStoreVector3(&normalizedDirection, XMVector3Normalize(direction_v));

        auto dotPointA = Dot(collider.pointA, normalizedDirection);
        auto dotPointB = Dot(collider.pointB, normalizedDirection);

        const auto& extremePoint = dotPointA > dotPointB ? collider.pointA : collider.pointB;

        auto out = extremePoint + normalizedDirection * collider.radius;
        return XMLoadVector3(&out);
    }

    XMVECTOR MinkowskiSupport(const HullCollider& collider, FXMVECTOR direction_v)
    {
        IF_THROW(collider.vertices.size() == 0u, "MinkowskiSupport - HullCollider vertex buffer is empty");
        const auto& vertices = collider.vertices;
        auto maxVertex_v = XMLoadVector3(&vertices[0]);
        auto maxDot_v = XMVector3Dot(maxVertex_v, direction_v);

        for(size_t i = 1u; i < vertices.size(); ++i)
        {
            auto vertex_v = XMLoadVector3(&vertices[i]);
            auto dot_v = XMVector3Dot(vertex_v, direction_v);
            if(XMVector3Greater(dot_v, maxDot_v))
            {
                maxDot_v = dot_v;
                maxVertex_v = vertex_v;
            }
        }

        return maxVertex_v;
    }

    XMVECTOR MinkowskiSupport(const BoundingVolume& collider, FXMVECTOR direction_v)
    {
        return std::visit([&direction_v](auto&& bv)
        {
            return MinkowskiSupport(bv, direction_v);
        }, collider);
    }

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
            simplex.ToTriangle<0u, 1u, 3u>();
            return RefineTriangle(simplex, direction);
        }

        return true;
    }
} // namespace nc