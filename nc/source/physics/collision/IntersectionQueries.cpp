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

    struct TriangleXM
    {
        XMVECTOR a, b, c;
    };

    struct Supports
    {
        XMVECTOR localA, localB, worldA, worldB, worldCSO;
    };

    constexpr float FloatMax = std::numeric_limits<float>::max();
    constexpr float EpaTolerance = 0.001f;
    constexpr size_t GjkMaxIterations = 10u;
    constexpr size_t EpaMaxIterations = 32u;

    bool Collide(const Sphere& a, const Sphere& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut);

    template<bool GenerateContacts, class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut);

    template<class BVA>
    bool GjkVsTriangle(const BVA& a, const Triangle& b, FXMMATRIX aMatrix, CollisionState* stateOut);

    template<class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b);

    template<class BVA, class BVB>
    bool Epa(const BVA& a, const BVB& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut);

    template<class BVA>
    bool EpaVsTriangle(const BVA& a, const TriangleXM& b, FXMMATRIX aMatrix, CollisionState* stateOut);

    bool SameDirection(const Vector3& a, const Vector3& b);
    bool RefinePoint(Simplex& simplex, Vector3& direction);
    bool RefineLine(Simplex& simplex, Vector3& direction);
    bool RefineTriangle(Simplex& simplex, Vector3& direction);
    bool RefineTetrahedron(Simplex& simplex, Vector3& direction);
    XMVECTOR GetTranslation(FXMMATRIX matrix);
    XMVECTOR GetScale(FXMMATRIX matrix);
    XMVECTOR GetRotation(FXMMATRIX matrix);
    float SquareMtdToAABB(const Vector3& point, const Box& aabb);
    XMVECTOR MinkowskiSupport(const Sphere& collider, FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const Box& collider, FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const Capsule& collider, FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const ConvexHull& collider, FXMVECTOR direction_v);
    XMVECTOR MinkowskiSupport(const TriangleXM& triangle, FXMVECTOR direction_v);

    template<class BVA, class BVB>
    Supports MinkowskiSupports(const BVA& a, const BVB& b, FXMVECTOR aRot, FXMVECTOR bRot, FXMMATRIX aMatrix, FXMMATRIX bMatrix, FXMVECTOR direction);

    constexpr std::array<bool(*)(Simplex&, Vector3&), 4u> RefineSimplex = 
    {
        RefinePoint, RefineLine, RefineTriangle, RefineTetrahedron
    };

    auto TestHalfspace(const Plane& plane, const Sphere& sphere) -> HalfspaceContainment
    {
        float dist = Dot(plane.normal, sphere.center) - plane.d;

        if(std::abs(dist) <= sphere.radius)
            return HalfspaceContainment::Intersecting;
        else if(dist > 0.0f)
            return HalfspaceContainment::Positive;
        
        return HalfspaceContainment::Negative;
    }

    bool Intersect(const Sphere& a, const Sphere& b)
    {
        auto radii = a.radius + b.radius;
        return SquareMagnitude(a.center - b.center) < radii * radii;
    }

    bool Intersect(const Frustum& a, const Sphere& b)
    {
        if(auto space = TestHalfspace(a.back, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.left, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.right, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.top, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.bottom, b); space != HalfspaceContainment::Positive)
            return space == HalfspaceContainment::Intersecting ? true : false;

        if(auto space = TestHalfspace(a.front, b); space != HalfspaceContainment::Positive)
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
        return std::visit([&aMatrix, &bMatrix](auto&& bva, auto&& bvb)
        {
            using bva_t = std::decay_t<decltype(bva)>;
            using bvb_t = std::decay_t<decltype(bvb)>;
            CollisionState state;

            if constexpr(std::is_same_v<bva_t, Sphere> && std::is_same_v<bvb_t, Sphere>)
            {
                return Collide(bva, bvb, aMatrix, bMatrix, &state);
            }
            else
            {
                return Gjk<false>(bva, bvb, aMatrix, bMatrix, &state);
            }
        }, a, b);
    }

    bool Collide(const BoundingVolume& a, const BoundingVolume& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        return std::visit([&aMatrix, &bMatrix, stateOut](auto&& bva, auto&& bvb)
        {
            using bva_t = std::decay_t<decltype(bva)>;
            using bvb_t = std::decay_t<decltype(bvb)>;

            if constexpr(std::is_same_v<bva_t, Sphere> && std::is_same_v<bvb_t, Sphere>)
            {
                return Collide(bva, bvb, aMatrix, bMatrix, stateOut);
            }
            else
            {
                return Gjk<true>(bva, bvb, aMatrix, bMatrix, stateOut);
            }
        }, a, b);
    }

    bool Collide(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut)
    {
        return std::visit([&b, &aMatrix, stateOut](auto&& a)
        {
            return GjkVsTriangle(a, b, aMatrix, stateOut);
        }, a);
    }

    XMVECTOR GetTranslation(FXMMATRIX matrix)
    {
        return matrix.r[3];
    }

    XMVECTOR GetScale(FXMMATRIX matrix)
    {
        auto x = DirectX::XMVector3Dot(matrix.r[0], matrix.r[0]);
        auto y = DirectX::XMVector3Dot(matrix.r[1], matrix.r[1]);
        auto z = DirectX::XMVector3Dot(matrix.r[2], matrix.r[2]);

        // could use mergeXY + permute
        return XMVectorSet(XMVectorGetX(x), XMVectorGetX(y), XMVectorGetX(z), 0.0f);
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

    XMVECTOR MinkowskiSupport(const Sphere& collider, FXMVECTOR direction_v)
    {
        return XMLoadVector3(&collider.center) +
               XMVectorScale(XMVector3Normalize(direction_v), collider.radius);
    }

    XMVECTOR MinkowskiSupport(const Box& collider, FXMVECTOR direction_v)
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

    XMVECTOR MinkowskiSupport(const Capsule& collider, FXMVECTOR direction_v)
    {
        Vector3 normalizedDirection;
        XMStoreVector3(&normalizedDirection, XMVector3Normalize(direction_v));

        auto dotPointA = Dot(collider.pointA, normalizedDirection);
        auto dotPointB = Dot(collider.pointB, normalizedDirection);

        const auto& extremePoint = dotPointA > dotPointB ? collider.pointA : collider.pointB;

        auto out = extremePoint + normalizedDirection * collider.radius;
        return XMLoadVector3(&out);
    }

    XMVECTOR MinkowskiSupport(const ConvexHull& collider, FXMVECTOR direction_v)
    {
        IF_THROW(collider.vertices.size() == 0u, "MinkowskiSupport - ConvexHull vertex buffer is empty");
        const auto& vertices = collider.vertices;
        Vector3 direction;
        XMStoreVector3(&direction, direction_v);
        size_t maxIndex = 0u;
        float maxDot = Dot(vertices[maxIndex], direction);

        for(size_t i = 1u; i < vertices.size(); ++i)
        {
            float dot = Dot(vertices[i], direction);
            if(dot > maxDot)
            {
                maxDot = dot;
                maxIndex = i;
            }
        }

        return XMLoadVector3(&vertices[maxIndex]);
    }

    XMVECTOR MinkowskiSupport(const TriangleXM& triangle, FXMVECTOR direction_v)
    {
        auto dotA = XMVector3Dot(triangle.a, direction_v);
        auto dotB = XMVector3Dot(triangle.b, direction_v);
        auto dotC = XMVector3Dot(triangle.c, direction_v);

        if(XMVector3Greater(dotA, dotB))
        {
            return XMVector3Greater(dotA, dotC) ? triangle.a : triangle.c;
        }

        return XMVector3Greater(dotB, dotC) ? triangle.b : triangle.c;
    }

    template<class BVA, class BVB>
    Supports MinkowskiSupports(const BVA& a, const BVB& b, FXMVECTOR aRot, FXMVECTOR bRot, FXMMATRIX aMatrix, FXMMATRIX bMatrix, FXMVECTOR direction)
    {
        Supports out;
        out.localA = MinkowskiSupport(a, XMVector3InverseRotate(direction, aRot));
        out.worldA = XMVector3Transform(out.localA, aMatrix);
        out.localB = MinkowskiSupport(b, XMVector3InverseRotate(XMVectorNegate(direction), bRot));
        out.worldB = XMVector3Transform(out.localB, bMatrix);
        out.worldCSO = out.worldA - out.worldB;
        return out;
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

    bool Collide(const Sphere& a, const Sphere& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
    {
        auto translationA = GetTranslation(aMatrix);
        auto scaleA = GetScale(aMatrix);
        auto centerA = XMLoadVector3(&a.center) + translationA;
        auto radiusA = XMVectorScale(scaleA, a.radius);

        auto translationB = GetTranslation(bMatrix);
        auto scaleB = GetScale(bMatrix);
        auto centerB = XMLoadVector3(&b.center) + translationB;
        auto radiusB = XMVectorScale(scaleB, b.radius);

        auto sqDist = XMVector3LengthSq(centerA - centerB);
        auto sqRadii = radiusA + radiusB;
        sqRadii = sqRadii * sqRadii;

        if(XMVector3Greater(sqDist, sqRadii))
            return false;
        
        auto ab = centerB - centerA;
        auto normal = XMVector3Normalize(ab);
        float depth = XMVectorGetX(sqRadii - sqDist);
        auto worldContactA_v = centerA + normal * radiusA;
        auto worldContactB_v = centerB - normal * radiusB;

        XMStoreVector3(&stateOut->contact.normal, normal);
        stateOut->contact.depth = depth;
        stateOut->contact.localPointA = a.center + stateOut->contact.normal * a.radius;
        stateOut->contact.localPointB = b.center + stateOut->contact.normal * b.radius;
        XMStoreVector3(&stateOut->contact.worldPointA, worldContactA_v);
        XMStoreVector3(&stateOut->contact.worldPointB, worldContactB_v);
        
        return true;
    }

    template<bool GenerateContacts, class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b, FXMMATRIX aMatrix, FXMMATRIX bMatrix, CollisionState* stateOut)
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
                if constexpr(GenerateContacts)
                    return Epa<BVA, BVB>(a, b, aMatrix, bMatrix, stateOut);
                else
                    return true;
            }
        }

        return false;
    }

    template<class BVA>
    bool GjkVsTriangle(const BVA& a, const Triangle& b, FXMMATRIX aMatrix, CollisionState* stateOut)
    {
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
            auto supportCSO_v = aSupportWorld_v - bSupportWorld_v;

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
                return EpaVsTriangle(a, bXM, aMatrix, stateOut);
            }
        }

        return false;
    }

    template<class BVA, class BVB>
    bool Gjk(const BVA& a, const BVB& b)
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

    template<class BVA, class BVB>
    bool Epa(const BVA& a, const BVB& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state)
    {
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
            auto supports = MinkowskiSupports(a, b, state->rotationA, state->rotationB, aMatrix, bMatrix, direction_v);

            Vector3 support;
            XMStoreVector3(&support, supports.worldCSO);
            XMStoreVector3(&(state->contact.worldPointA), supports.worldA);
            XMStoreVector3(&(state->contact.worldPointB), supports.worldB);
            XMStoreVector3(&(state->contact.localPointA), supports.localA);
            XMStoreVector3(&(state->contact.localPointB), supports.localB);

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
                    state->contact.lambda = 0.0f;
                    state->contact.muTangent = 0.0f;
                    state->contact.muBitangent = 0.0f;
                    return false;
                }

                minNorm.distance = FloatMax;
            }
        }

        auto success = state->polytope.GetContacts(minFace, &state->contact);
        state->contact.normal = Normalize(minNorm.normal);
        state->contact.depth = minNorm.distance + EpaTolerance;
        state->contact.lambda = 0.0f;
        state->contact.muTangent = 0.0f;
        state->contact.muBitangent = 0.0f;
        return success;
    }

    template<class BVA>
    bool EpaVsTriangle(const BVA& a, const TriangleXM& b, FXMMATRIX aMatrix, CollisionState* stateOut)
    {
        stateOut->polytope.Initialize(stateOut->simplex);
        auto minFace = stateOut->polytope.ComputeNormalData();
        NormalData minNorm{Vector3::Zero(), FloatMax};
        size_t iterations = 0u;

        while(minNorm.distance == FloatMax)
        {
            // Normal data for the face closest to the origin.
            minNorm = stateOut->polytope.GetNormalData(minFace);

            if(iterations++ > EpaMaxIterations)
                break;

            // Find a point on the Minkowski hull in the direction of the closest face's normal.
            auto direction_v = XMLoadVector3(&minNorm.normal);
            auto aDirection_v = XMVector3InverseRotate(direction_v, stateOut->rotationA);
            auto aSupportLocal_v = MinkowskiSupport(a, aDirection_v);
            auto aSupportWorld_v = XMVector3Transform(aSupportLocal_v, aMatrix);

            auto bDirection_v = XMVectorNegate(direction_v);
            auto bSupportWorld_v = MinkowskiSupport(b, bDirection_v);
            
            auto support_v = aSupportWorld_v - bSupportWorld_v;
            Vector3 support;
            XMStoreVector3(&support, support_v);

            XMStoreVector3(&(stateOut->contact.worldPointA), aSupportWorld_v);
            XMStoreVector3(&(stateOut->contact.worldPointB), bSupportWorld_v);
            XMStoreVector3(&(stateOut->contact.localPointA), aSupportLocal_v);
            XMStoreVector3(&(stateOut->contact.localPointB), bSupportWorld_v);

            if(abs(Dot(minNorm.normal, support) - minNorm.distance) > EpaTolerance)
            {
                // The closest face is not on the hull, so we expand towards the hull.
                if(!stateOut->polytope.Expand(support, stateOut->contact, &minFace))
                {
                    /** @todo Need to determine if this can happen under normal circumstances.
                     *  It has thrown here a few times, but always when there is wonk elsewhere. */
                    //throw std::runtime_error("Epa - minDistance not found");
                    std::cout << "Epa - minDistance not found\n";
                    stateOut->contact.depth = 0.0f;
                    stateOut->contact.lambda = 0.0f;
                    stateOut->contact.muTangent = 0.0f;
                    stateOut->contact.muBitangent = 0.0f;
                    return false;
                }

                minNorm.distance = FloatMax;
            }
        }

        auto success = stateOut->polytope.GetContacts(minFace, &stateOut->contact);
        stateOut->contact.normal = Normalize(minNorm.normal);
        stateOut->contact.depth = minNorm.distance + EpaTolerance;
        stateOut->contact.lambda = 0.0f;
        stateOut->contact.muTangent = 0.0f;
        stateOut->contact.muBitangent = 0.0f;
        return success;
    }
} // namespace nc