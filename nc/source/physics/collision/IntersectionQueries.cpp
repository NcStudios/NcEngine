#include "IntersectionQueries.h"
#include "narrow_phase/Gjk.h"
#include "debug/Profiler.h"

#include <algorithm>

#include "debug/Serialize.h"

using namespace DirectX;

namespace
{
    using namespace nc;
    using namespace nc::physics;

    [[maybe_unused]]
    float SquareMtdToAABB(const Vector3& point, const Box& aabb)
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

    template<bool GenerateContacts>
    bool SphereVsSphere(const Sphere& a, const Sphere& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, [[maybe_unused]] CollisionState* stateOut)
    {
        const auto centerA = XMLoadVector3(&a.center) + aMatrix.r[3];
        const auto radiusA = XMVectorScale(GetScale(aMatrix), a.radius);
        const auto centerB = XMLoadVector3(&b.center) + bMatrix.r[3];
        const auto radiusB = XMVectorScale(GetScale(bMatrix), b.radius);

        const auto ab = centerB - centerA;
        const auto sqDist = XMVector3LengthSq(ab);
        auto sqRadii = radiusA + radiusB;
        sqRadii = sqRadii * sqRadii;

        if(XMVector3Greater(sqDist, sqRadii))
            return false;
        
        if constexpr(GenerateContacts)
        {
            stateOut->contact.depth = XMVectorGetX(sqRadii - sqDist);
            const auto normal = XMVector3Normalize(ab);
            XMStoreVector3(&stateOut->contact.normal, normal);
            XMStoreVector3(&stateOut->contact.worldPointA, centerA + normal * radiusA);
            XMStoreVector3(&stateOut->contact.worldPointB, centerB - normal * radiusB);
            stateOut->contact.localPointA = a.center + stateOut->contact.normal * a.radius;
            stateOut->contact.localPointB = b.center + stateOut->contact.normal * b.radius;
        }

        return true;
    }

    template<bool GenerateContacts>
    bool BoxVsSphere(const Box& box, const Sphere& sphere, FXMMATRIX boxMatrix, FXMMATRIX sphereMatrix, CollisionState* stateOut)
    {
        XMVECTOR boxScl_v, boxRot_v, boxPos_v;
        XMMatrixDecompose(&boxScl_v, &boxRot_v, &boxPos_v, boxMatrix);
        const auto boxExtents_v = boxScl_v * XMVectorScale(XMLoadVector3(&box.extents), 0.5f);
        XMVECTOR boxExtents[3] = { XMVectorSplatX(boxExtents_v), XMVectorSplatY(boxExtents_v), XMVectorSplatZ(boxExtents_v) };
        XMVECTOR boxAxes[3] = { XMVector3Rotate(g_XMIdentityR0, boxRot_v), XMVector3Rotate(g_XMIdentityR1, boxRot_v), XMVector3Rotate(g_XMIdentityR2, boxRot_v) };
        const auto boxCenter_v = boxPos_v + XMLoadVector3(&box.center);

        const auto sphereCenter_v = XMLoadVector3(&box.center) + sphereMatrix.r[3];
        const auto sphereRadius = XMVectorScale(GetScale(sphereMatrix), sphere.radius);

        const auto direction = sphereCenter_v - boxCenter_v;
        auto closestPointOnBox = boxCenter_v;

        for(size_t i = 0u; i < 3u; ++i)
        {
            auto axisProjection = XMVector3Dot(direction, boxAxes[i]);
            if(XMVector3Greater(axisProjection, boxExtents[i]))
            {
                axisProjection = boxExtents[i];
            }
            else
            {
                auto negExtent = XMVectorNegate(boxExtents[i]);
                if(XMVector3Less(axisProjection, negExtent))
                {
                    axisProjection = negExtent;
                }
            }

            closestPointOnBox += boxAxes[i] * axisProjection;
        }

        auto pointToSphere = sphereCenter_v - closestPointOnBox;
        auto squareMag = XMVector3Dot(pointToSphere, pointToSphere);
        if(XMVector3Greater(squareMag, sphereRadius * sphereRadius))
            return false;

        if constexpr(GenerateContacts)
        {
            auto normal_v = XMVector3Normalize(pointToSphere);
            auto pointOnSphere = sphereCenter_v - normal_v * sphereRadius;
            auto depth_v = XMVector3Length(closestPointOnBox - pointOnSphere);
            XMStoreVector3(&stateOut->contact.normal, normal_v);
            stateOut->contact.depth = XMVectorGetX(depth_v);
            XMStoreVector3(&stateOut->contact.worldPointA, closestPointOnBox);
            XMStoreVector3(&stateOut->contact.worldPointB, pointOnSphere);
            auto boxLocal = XMVector3Transform(closestPointOnBox, XMMatrixInverse(nullptr, boxMatrix));
            XMStoreVector3(&stateOut->contact.localPointA, boxLocal);
            stateOut->contact.localPointB = sphere.center - stateOut->contact.normal * sphere.radius;


            // std::cout << "worldA: " << stateOut->contact.worldPointA << '\n'
            //           << "localA: " << stateOut->contact.localPointA << '\n'
            //           << "worldB: " << stateOut->contact.worldPointB << '\n'
            //           << "localB: " << stateOut->contact.localPointB << '\n'
            //           << "center: " << sphereCenter_v
            //           << "redius: " << sphereRadius
            //           << "GetScale: " << GetScale(sphereMatrix)
            //           << "sphere.radius: " << sphere.radius << '\n'
            //           << "norm: " << stateOut->contact.normal << '\n'
            //           << "dept: " << stateOut->contact.depth << '\n';
        }

        return true;
    }

    template<bool GenerateContacts>
    bool SphereVsBox(const Sphere& sphere, const Box& box, FXMMATRIX sphereMatrix, FXMMATRIX boxMatrix, CollisionState* stateOut)
    {
        if(!BoxVsSphere<GenerateContacts>(box, sphere, boxMatrix, sphereMatrix, stateOut))
        {
            return false;
        }

        if constexpr(GenerateContacts)
        {
            auto tempWorld = stateOut->contact.worldPointA;
            stateOut->contact.worldPointA = stateOut->contact.worldPointB;
            stateOut->contact.worldPointB = tempWorld;

            auto tempLocal = stateOut->contact.localPointA;
            stateOut->contact.localPointA = stateOut->contact.localPointB;
            stateOut->contact.localPointB = tempLocal;

            stateOut->contact.normal = -stateOut->contact.normal;
        }

        return true;
    }

    template<bool GenerateContacts>
    bool CollisionDispatch(const BoundingVolume& a, const BoundingVolume& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        return std::visit([&aMatrix, &bMatrix, stateOut](auto&& bva, auto&& bvb)
        {
            using bva_t = std::decay_t<decltype(bva)>;
            using bvb_t = std::decay_t<decltype(bvb)>;

            if constexpr(std::is_same_v<bva_t, Sphere> && std::is_same_v<bvb_t, Sphere>)
            {
                return SphereVsSphere<GenerateContacts>(bva, bvb, aMatrix, bMatrix, stateOut);
            }
            else if constexpr(std::is_same_v<bva_t, Sphere> && std::is_same_v<bvb_t, Box>)
            {
                return SphereVsBox<GenerateContacts>(bva, bvb, aMatrix, bMatrix, stateOut);
            }
            else if constexpr(std::is_same_v<bva_t, Box> && std::is_same_v<bvb_t, Sphere>)
            {
                return BoxVsSphere<GenerateContacts>(bva, bvb, aMatrix, bMatrix, stateOut);
            }
            else
            {
                return Gjk<GenerateContacts>(bva, bvb, aMatrix, bMatrix, stateOut);
            }
        }, a, b);
    }
}

namespace nc::physics
{
    bool Intersect(const Sphere& a, const Sphere& b)
    {
        auto radii = a.radius + b.radius;
        return SquareMagnitude(a.center - b.center) < radii * radii;
    }

    bool Intersect(const Sphere& a, const Box& b)
    {
        return SquareMtdToAABB(a.center, b) <= a.radius * a.radius;
    }

    bool Intersect(const Frustum& a, const Sphere& b)
    {
        if(auto space = TestHalfspace(a.front, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.left, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.right, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.top, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.bottom, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.back, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        return true;
    }

    bool Intersect(const BoundingVolume& a, const BoundingVolume& b)
    {
        return std::visit([](auto&& bva, auto&& bvb)
        {
            return Gjk(bva, bvb);
        }, a, b);
    }

    bool Intersect(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix)
    {
        CollisionState state;
        return CollisionDispatch<false>(a, b, aMatrix, bMatrix, &state);
    }

    bool Collide(const BoundingVolume& a, const BoundingVolume& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        return CollisionDispatch<true>(a, b, aMatrix, bMatrix, stateOut);
    }

    bool Collide(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut)
    {
        return std::visit([&b, &aMatrix, stateOut](auto&& a)
        {
            return GjkVsTriangle(a, b, aMatrix, stateOut);
        }, a);
    }

    auto TestHalfspace(const Plane& plane, const Sphere& sphere) -> HalfspaceContainment
    {
        float dist = Dot(plane.normal, sphere.center) - plane.d;

        if(std::abs(dist) <= sphere.radius)
            return HalfspaceContainment::Intersecting;
        else if(dist > 0.0f)
            return HalfspaceContainment::Positive;
        
        return HalfspaceContainment::Negative;
    }
} // namespace nc