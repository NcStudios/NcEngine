/**
 * @file SpotLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
/** @brief Component representing a light pointing in a single direction with a radius. */
struct SpotLight final : public ComponentBase
{
    explicit SpotLight(Entity entity,
                       const Vector3& direction_ = Vector3::Front(),
                       const Vector3& color_ = Vector3{1.0f, 0.9f, 0.9f},
                       float radius_ = 20.0f) noexcept
        : ComponentBase{entity},
          direction{direction_},
          color{color_},
          radius{radius_}
    {
    }
    Vector3 direction;
    Vector3 color;
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
