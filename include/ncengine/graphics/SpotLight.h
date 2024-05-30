/**
 * @file SpotLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
/** @brief Component representing a light pointing in a single direction with an angle specifying the radius. */
struct SpotLight final : public ComponentBase
{
    explicit SpotLight(Entity entity,
                       const Vector3& color_ = Vector3{1.0f, 0.9f, 0.9f},
                       float innerAngle_ = 0.3491f,
                       float outerAngle_ = 0.5236f) noexcept
        : ComponentBase{entity},
          color{color_},
          innerAngle{innerAngle_},
          outerAngle{outerAngle_}
    {
    }
    Vector3 color;
    float innerAngle;
    float outerAngle;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::SpotLight> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = false;
    static constexpr bool EnableOnCommitCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
