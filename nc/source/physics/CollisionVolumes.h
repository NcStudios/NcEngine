#pragma once

#include "math/Vector3.h"
#include "component/Collider.h"
#include "directx/math/DirectXMath.h"

#include <span>
#include <variant>

namespace nc::physics
{
    struct SphereCollider
    {
        Vector3 center;
        float radius;
    };

    struct BoxCollider
    {
        Vector3 center;
        Vector3 extents;
        float maxExtent;
    };

    struct CapsuleCollider
    {
        Vector3 pointA, pointB;
        float radius;
        float maxExtent;
    };

    struct HullCollider
    {
        std::span<const Vector3> vertices;
        float maxExtent;
    };

    using BoundingVolume = std::variant<BoxCollider, SphereCollider, CapsuleCollider, HullCollider>;

    auto CreateBoundingVolume(const Collider::VolumeInfo& info) -> BoundingVolume;
    auto EstimateBoundingVolume(const BoundingVolume& volume, DirectX::FXMMATRIX matrix) -> SphereCollider;
} // namespace nc::physics