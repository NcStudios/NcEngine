/**
 * @file PointLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
/** @brief Component representing an omnidirectional light. */
struct PointLight final : public ComponentBase
{
    explicit PointLight(Entity entity,
                        const Vector3& ambientColor_ = Vector3{0.25f, 0.25f, 0.25f},
                        const Vector3& diffuseColor_ = Vector3{1.0f, 0.9f, 0.9f},
                        float radius_ = 25.0f) noexcept
        : ComponentBase{entity},
          ambientColor{ambientColor_},
          diffuseColor{diffuseColor_},
          radius{radius_}
    {
    }

    Vector3 ambientColor;
    Vector3 diffuseColor;
    float radius;
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
