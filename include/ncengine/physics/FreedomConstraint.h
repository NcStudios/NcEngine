#pragma once

#include "ncengine/utility/MatrixUtilities.h"

namespace nc::physics
{
/** @brief Component for constraining degrees of freedom within NcPhysics.
 * 
 * Linear and angular components may be initialized to 0 or 1 to enable or disable physics-based
 * motion on a per-axis basis. A FreedomConstraint acts upon an object's velocity and does not
 * restrict translations or rotations applied directly to a Transform. It has no effect if the
 * parent Entity does not have a have a PhysicsBody.
*/
struct FreedomConstraint
{
    explicit FreedomConstraint(const Vector3& linearFreedom_ = Vector3::One(),
                               const Vector3& angularFreedom_ = Vector3::One(),
                               bool worldSpace_ = true) noexcept
        : linearFreedom{ToXMVector(linearFreedom_)},
          angularFreedom{ToXMVector(angularFreedom_)},
          worldSpace{worldSpace_}
    {
    }

    DirectX::XMVECTOR linearFreedom;
    DirectX::XMVECTOR angularFreedom;
    bool worldSpace;
};
} // namespace nc::physics
