#pragma once

#include "component/Collider.h"

#include "math/Quaternion.h"

#include "directx/math/DirectXMath.h"
#include <algorithm>
#include <array>
#include <vector>
#include <stdexcept>

#include <variant>

namespace nc::physics
{
    bool BroadCollision(const SphereCollider& a,
                        const SphereCollider& b);

    /* Intersection query between two untransformed volumes. */
    bool GJK(const Collider::BoundingVolume& a,
             const Collider::BoundingVolume& b,
             DirectX::FXMMATRIX aMatrix,
             DirectX::FXMMATRIX bMatrix);
            
    /* Intersection query between two transformed volumes. */
    bool GJK(const Collider::BoundingVolume& a,
             const Collider::BoundingVolume& b);
} // namespace nc::physics