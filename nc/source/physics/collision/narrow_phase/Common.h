#pragma once

#include "debug/Utils.h"
#include "directx/math/DirectXMath.h"

#include <array>

namespace nc::physics
{
    struct TriangleXM
    {
        DirectX::XMVECTOR a, b, c;
    };

    struct Supports
    {
        DirectX::XMVECTOR localA, localB, worldA, worldB, worldCSO;
    };

    bool SameDirection(const Vector3& a, const Vector3& b);
    DirectX::XMVECTOR GetTranslation(DirectX::FXMMATRIX matrix);
    DirectX::XMVECTOR GetScale(DirectX::FXMMATRIX matrix);
    DirectX::XMVECTOR GetRotation(DirectX::FXMMATRIX matrix);
    DirectX::XMVECTOR MinkowskiSupport(const Sphere& collider, DirectX::FXMVECTOR direction_v);
    DirectX::XMVECTOR MinkowskiSupport(const Box& collider, DirectX::FXMVECTOR direction_v);
    DirectX::XMVECTOR MinkowskiSupport(const Capsule& collider, DirectX::FXMVECTOR direction_v);
    DirectX::XMVECTOR MinkowskiSupport(const ConvexHull& collider, DirectX::FXMVECTOR direction_v);
    DirectX::XMVECTOR MinkowskiSupport(const TriangleXM& triangle, DirectX::FXMVECTOR direction_v);

    template<class BVA, class BVB>
    Supports MinkowskiSupports(const BVA& a,
                               const BVB& b,
                               DirectX::FXMVECTOR aRot,
                               DirectX::FXMVECTOR bRot,
                               DirectX::FXMMATRIX aMatrix,
                               DirectX::FXMMATRIX bMatrix,
                               DirectX::FXMVECTOR direction);

    inline bool SameDirection(const Vector3& a, const Vector3& b)
    {
        return Dot(a, b) > 0.0f;
    }

    inline DirectX::XMVECTOR GetTranslation(DirectX::FXMMATRIX matrix)
    {
        return matrix.r[3];
    }

    inline DirectX::XMVECTOR GetScale(DirectX::FXMMATRIX matrix)
    {
        using namespace DirectX;

        const auto x = XMVector3LengthSq(matrix.r[0]);
        const auto y = XMVector3LengthSq(matrix.r[1]);
        const auto z = XMVector3LengthSq(matrix.r[2]);
        auto out = XMVectorMergeXY(x, y);
        out = XMVectorPermute<0, 1, 4, 5>(out, z);
        return XMVectorSqrt(out);
    }

    inline DirectX::XMVECTOR GetRotation(DirectX::FXMMATRIX matrix)
    {
        DirectX::XMVECTOR scl, rot, pos;
        DirectX::XMMatrixDecompose(&scl, &rot, &pos, matrix);
        return rot;
    }

    inline DirectX::XMVECTOR MinkowskiSupport(const Sphere& collider, DirectX::FXMVECTOR direction_v)
    {
        return DirectX::XMLoadVector3(&collider.center) +
               DirectX::XMVectorScale(DirectX::XMVector3Normalize(direction_v), collider.radius);
    }

    inline DirectX::XMVECTOR MinkowskiSupport(const Box& collider, DirectX::FXMVECTOR direction_v)
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

    inline DirectX::XMVECTOR MinkowskiSupport(const Capsule& collider, DirectX::FXMVECTOR direction_v)
    {
        Vector3 normalizedDirection;
        DirectX::XMStoreVector3(&normalizedDirection, DirectX::XMVector3Normalize(direction_v));

        auto dotPointA = Dot(collider.pointA, normalizedDirection);
        auto dotPointB = Dot(collider.pointB, normalizedDirection);

        const auto& extremePoint = dotPointA > dotPointB ? collider.pointA : collider.pointB;

        auto out = extremePoint + normalizedDirection * collider.radius;
        return DirectX::XMLoadVector3(&out);
    }

    inline DirectX::XMVECTOR MinkowskiSupport(const ConvexHull& collider, DirectX::FXMVECTOR direction_v)
    {
        IF_THROW(collider.vertices.size() == 0u, "MinkowskiSupport - ConvexHull vertex buffer is empty");
        const auto& vertices = collider.vertices;
        Vector3 direction;
        DirectX::XMStoreVector3(&direction, direction_v);
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

        return DirectX::XMLoadVector3(&vertices[maxIndex]);
    }

    inline DirectX::XMVECTOR MinkowskiSupport(const TriangleXM& triangle, DirectX::FXMVECTOR direction_v)
    {
        auto dotA = DirectX::XMVector3Dot(triangle.a, direction_v);
        auto dotB = DirectX::XMVector3Dot(triangle.b, direction_v);
        auto dotC = DirectX::XMVector3Dot(triangle.c, direction_v);

        if(DirectX::XMVector3Greater(dotA, dotB))
        {
            return DirectX::XMVector3Greater(dotA, dotC) ? triangle.a : triangle.c;
        }

        return DirectX::XMVector3Greater(dotB, dotC) ? triangle.b : triangle.c;
    }

    template<class BVA, class BVB>
    Supports MinkowskiSupports(const BVA& a,
                               const BVB& b,
                               DirectX::FXMVECTOR aRot,
                               DirectX::FXMVECTOR bRot,
                               DirectX::FXMMATRIX aMatrix,
                               DirectX::FXMMATRIX bMatrix,
                               DirectX::FXMVECTOR direction)
    {
        using namespace DirectX;

        Supports out;
        out.localA = MinkowskiSupport(a, XMVector3InverseRotate(direction, aRot));
        out.worldA = XMVector3Transform(out.localA, aMatrix);
        out.localB = MinkowskiSupport(b, XMVector3InverseRotate(XMVectorNegate(direction), bRot));
        out.worldB = XMVector3Transform(out.localB, bMatrix);
        out.worldCSO = out.worldA - out.worldB;
        return out;
    }
}