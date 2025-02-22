/**
 * @file PostProcess.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"

#include <limits>
#include <variant>

namespace nc
{
/** @brief Post process pass flags */
struct PostProcessPassFlag
{
    using type = uint64_t;

    static constexpr auto None     = type{0};
    static constexpr auto Outline  = type{1 << 0};
    static constexpr auto Fxaa     = type{2 << 0};
    static constexpr auto Gradient = type{3 << 0};
};

/** @brief Identifier for a post process effect. */
using PostProcessEffectId = uint32_t;

/** @brief Set of flags indicating the passes used by a post process effect. */
using PostProcessEffectPassFlags = PostProcessPassFlag::type;

/** @brief Null identifier for a post process effect. */
constexpr auto NullPostProcessEffectId = std::numeric_limits<PostProcessEffectId>::max();

/** @brief Identifier for the outlined toon post process effect. */
constexpr auto OutlinedToonEffectId = PostProcessEffectId{0};

/** @brief Pass flags for the outlined toon post process effect. */
constexpr auto OutlinedToonEffectPassFlags = PostProcessPassFlag::Outline | PostProcessPassFlag::Fxaa | PostProcessPassFlag::Gradient;

/** @brief Post process property type representing an empty or uninitialized state. */
struct EmptyPassProperties {};

/** @brief Properties for the outline pass. */
struct OutlinePassProperties
{
    Vector3 color = Vector3::Zero();
    float width = 1.0f;
    float depthThreshold = 0.8f;
    float viewDirDepthThreshold = 0.4f;
    float normalThreshold = 0.4f;
};

/** @brief Properties for the gradient pass. */
struct GradientPassProperties
{
    Vector3 gradientStart = Vector3::Zero();
    float gradientAmount = 0.1f;
    Vector3 gradientEnd = Vector3::One();
    uint32_t noiseTexIndex = 0u;
    float noiseTexAmount = 0.1f;
    float noiseTexTiling = 1.0f;
};

/** @brief Generic post process property type. */
using PostProcessPassProperties = std::variant<EmptyPassProperties,
                                               OutlinePassProperties,
                                               GradientPassProperties>;

/** @brief Returns if a post process pass has a property type. */
auto PassHasProperties(PostProcessPassFlag::type pass) -> bool;

/** @brief Construct a PostProcessPassProperties holding the property type for a pass. */
auto MakeDefaultPassProperties(PostProcessPassFlag::type pass) -> PostProcessPassProperties;
} // namespace nc
