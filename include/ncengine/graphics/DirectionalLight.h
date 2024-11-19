/**
 * @file DirectionalLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
/** @brief Component representing an infinite, directional light, like the sun. 
 * Not implementing shadowing currently due to this type of light's proclivity for artifacts. */
struct DirectionalLight
{
    explicit DirectionalLight(const Vector3& color_ = Vector3::One()) noexcept
        : color{color_}
    {
    }

    Vector3 color;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::DirectionalLight> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = false;
    static constexpr bool EnableOnCommitCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
