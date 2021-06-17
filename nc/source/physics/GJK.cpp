#include "GJK.h"
#include "Simplex.h"
#include "debug/Utils.h"

namespace nc::physics
{
    bool SameDirection(const Vector3& a, const Vector3& b)
    {
        return Dot(a, b) > 0.0f;
    }

    auto GetRotation(DirectX::FXMMATRIX matrix) -> DirectX::XMVECTOR;
    auto Support(const Collider::BoundingVolume& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR;
    auto Support(const SphereCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR;
    auto Support(const BoxCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR;
    auto Support(const MeshCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR;
    bool RefinePoint(Simplex& simplex, Vector3& direction);
    bool RefineLine(Simplex& points, Vector3& direction);
    bool RefineTriangle(Simplex& points, Vector3& direction);
    bool RefineTetrahedron(Simplex& points, Vector3& direction);

    constexpr std::array<bool(*)(Simplex&, Vector3&), 4u> RefineSimplex = 
    {
        RefinePoint, RefineLine, RefineTriangle, RefineTetrahedron
    };

    bool BroadCollision(const SphereCollider& a, const SphereCollider& b)
    {
        // can use dist squared?
        auto dist = Distance(a.center, b.center);
        return dist < a.radius + b.radius;
    }

    bool GJK(const Collider::BoundingVolume& a,
             const Collider::BoundingVolume& b,
             DirectX::FXMMATRIX aMatrix,
             DirectX::FXMMATRIX bMatrix)
    {
        auto aRotation = GetRotation(aMatrix);
        auto bRotation = GetRotation(bMatrix);

        Vector3 direction = Vector3::One();
        Simplex simplex;
        size_t itCount = 0;
        constexpr size_t maxIterations = 10u;

        while(++itCount <= maxIterations)
        {
            using namespace DirectX;

            const auto direction_v = XMLoadVector3(&direction);
            auto aDirection_v = XMVector3InverseRotate(direction_v, aRotation);
            auto aSupport_v = XMVector3Transform(Support(a, aDirection_v), aMatrix);
            auto bDirection_v = XMVector3InverseRotate(XMVectorNegate(direction_v), bRotation);
            auto bSupport_v = XMVector3Transform(Support(b, bDirection_v), bMatrix);
            auto support_v = aSupport_v - bSupport_v;

            if(XMVector3Less(XMVector3Dot(support_v, direction_v), g_XMZero))
                return false;

            Vector3 support;
            XMStoreVector3(&support, support_v);

            simplex.push_front(support);

            if(RefineSimplex[simplex.size() - 1](simplex, direction))
                return true;
        }

        return false;
    }

    bool GJK(const Collider::BoundingVolume& a,
             const Collider::BoundingVolume& b)
    {
        Vector3 direction = Vector3::One();
        Simplex simplex;
        size_t itCount = 0;
        constexpr size_t maxIterations = 10u;

        while(++itCount <= maxIterations)
        {
            using namespace DirectX;

            const auto direction_v = XMLoadVector3(&direction);
            auto support_v = Support(a, direction_v) - Support(b, XMVectorNegate(direction_v));

            if(XMVector3Less(XMVector3Dot(support_v, direction_v), g_XMZero))
                return false;

            Vector3 support;
            XMStoreVector3(&support, support_v);

            simplex.push_front(support);

            if(RefineSimplex[simplex.size() - 1](simplex, direction))
                return true;
        }

        return false;
    }

    DirectX::XMVECTOR GetRotation(DirectX::FXMMATRIX matrix)
    {
        DirectX::XMVECTOR scl, rot, pos;
        DirectX::XMMatrixDecompose(&scl, &rot, &pos, matrix);
        return rot;
    }

    auto Support(const SphereCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR
    {
        using namespace DirectX;
        return XMLoadVector3(&collider.center) +
               XMVectorScale(XMVector3Normalize(direction_v), collider.radius);
    }

    auto Support(const BoxCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR
    {
        Vector3 dir;
        DirectX::XMStoreVector3(&dir, direction_v);

        auto sign = [](float n) { return n < 0.0f ? -1.0f : 1.0f; };

        auto extent = Vector3
        {
            sign(dir.x) * collider.extents.x / 2.0f,
            sign(dir.y) * collider.extents.y / 2.0f,
            sign(dir.z) * collider.extents.z / 2.0f
        };

        dir = collider.center + extent;
        return DirectX::XMLoadVector3(&dir);
    }

    auto Support(const MeshCollider& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR
    {
        IF_THROW(collider.vertices.size() == 0u, "Gjk Support - MeshCollider vertex buffer is empty");
        const auto& vertices = collider.vertices;
        auto maxVertex_v = DirectX::XMLoadVector3(&vertices[0]);
        auto maxDot_v = DirectX::XMVector3Dot(maxVertex_v, direction_v);

        for(size_t i = 1u; i < vertices.size(); ++i)
        {
            auto vertex_v = DirectX::XMLoadVector3(&vertices[i]);
            auto dot_v = DirectX::XMVector3Dot(vertex_v, direction_v);
            if(DirectX::XMVector3Greater(dot_v, maxDot_v))
            {
                maxDot_v = dot_v;
                maxVertex_v = vertex_v;
            }
        }

        return maxVertex_v;
    }

    auto Support(const Collider::BoundingVolume& collider, DirectX::FXMVECTOR direction_v) -> DirectX::XMVECTOR
    {
        return std::visit([&direction_v](auto&& bv)
        {
            return Support(bv, direction_v);
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
            direction = CrossProduct(CrossProduct(ab, ao), ab);
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
                direction = CrossProduct(CrossProduct(ac, ao), ac);
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