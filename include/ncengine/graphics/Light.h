/**
 * @file PointLight.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc
{
/** @brief Component representing an infinite, directional light, like the sun. 
 * Not implementing shadowing currently due to this type of light's proclivity for artifacts. */
struct DirectionalLight
{
    Vector3 diffuseColor = Vector3::One();
    Vector3 specularColor = Vector3::One();
    Vector3 shadowColor = Vector3::Zero();
};

/** @brief Component representing an omnidirectional light. */
struct PointLight
{
    Vector3 diffuseColor = Vector3{1.0f, 0.9f, 0.9f};
    Vector3 specularColor = Vector3{0.25f, 0.25f, 0.25f};
    Vector3 shadowColor = Vector3::Zero();
    float radius = 25.0f;
};

/** @brief Component representing a light pointing in a single direction with a radius for attenuation, and an inner and outer angle to specify both cone size and falloff. */
struct SpotLight
{
    Vector3 diffuseColor = Vector3{1.0f, 0.9f, 0.9f};
    Vector3 specularColor = Vector3{1.0f, 0.9f, 0.9f};
    Vector3 shadowColor = Vector3::Zero();
    float innerAngle = 0.3491f;
    float outerAngle = 0.5236f;
    float radius = 25.0f;
};
} // namespace nc
