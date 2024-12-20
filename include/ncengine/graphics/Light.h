/**
 * @file Light.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"

#include <variant>

namespace nc::graphics
{
struct DirectionalLightProperties
{
    Vector3 color = Vector3{1.0f, 0.9f, 0.9f};
};

struct PointLightProperties
{
    Vector3 color = Vector3{1.0f, 0.9f, 0.9f};
    float radius = 25.0f;
};

struct SpotLightProperties
{
    Vector3 color = Vector3{1.0f, 0.9f, 0.9f};
    float innerAngle = 0.3491f;
    float outerAngle = 0.5236f;
    float radius = 25.0f;
};

using LightProperties = std::variant<DirectionalLightProperties,
                                     PointLightProperties,
                                     SpotLightProperties>;

struct Light
{
    explicit Light(const DirectionalLightProperties& properties_) noexcept
        : properties{properties_}
    {
    }

    explicit Light(const PointLightProperties& properties_) noexcept
        : properties{properties_}
    {
    }

    explicit Light(const SpotLightProperties& properties_) noexcept
        : properties{properties_}
    {
    }

    LightProperties properties;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::Light> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = false;
    static constexpr bool EnableOnCommitCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
