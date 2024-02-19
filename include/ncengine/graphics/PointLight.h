/**
 * @file PointLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
struct PointLight final : public ComponentBase
{
    explicit PointLight(Entity entity,
                        const Vector3& ambientColor_ = Vector3{1.0f, 0.9f, 0.9f},
                        const Vector3& diffuseColor_ = Vector3{1.0f, 0.9f, 0.9f},
                        float diffuseIntensity_ = 600.0f) noexcept
        : ComponentBase{entity},
          ambientColor{ambientColor_},
          diffuseColor{diffuseColor_},
          diffuseIntensity{diffuseIntensity_}
    {
    }

    Vector3 ambientColor;
    Vector3 diffuseColor;
    float diffuseIntensity;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::PointLight> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = false;
    static constexpr bool EnableOnCommitCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
