/**
 * @file SpotLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
/** @brief Component representing a light pointing in a single direction with a radius for attenuation, and an inner and outer angle to specify both cone size and falloff. */
struct SpotLight
{
    explicit SpotLight(const Vector3& color_ = Vector3{1.0f, 0.9f, 0.9f},
                       float innerAngle_ = 0.3491f,
                       float outerAngle_ = 0.5236f,
                       float radius_ = 25.0f) noexcept
        : color{color_},
          innerAngle{innerAngle_},
          outerAngle{outerAngle_},
          radius{radius_}
    {
    }
    Vector3 color;
    float innerAngle;
    float outerAngle;
    float radius;
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
