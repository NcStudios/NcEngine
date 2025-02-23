/**
 * @file PostProcess.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
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
    static constexpr auto Noise = type{3 << 0};
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
constexpr auto OutlinedToonEffectPassFlags = PostProcessPassFlag::Outline | PostProcessPassFlag::Fxaa | PostProcessPassFlag::Noise;

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

/** @brief Properties for the noise pass. */
struct NoisePassProperties
{
    Vector3 maskGradientStart = Vector3::Zero();
    float maskGradientAmount = 0.1f;
    Vector3 maskGradientEnd = Vector3::One();
    asset::TextureView noiseTex = asset::TextureView{};
    float noiseTexAmount = 0.1f;
    float noiseTexTiling = 1.0f;
};

/** @brief Generic post process property type. */
using PostProcessPassProperties = std::variant<EmptyPassProperties,
                                               OutlinePassProperties,
                                               NoisePassProperties>;

/** @brief Returns if a post process pass has a property type. */
auto PassHasProperties(PostProcessPassFlag::type pass) -> bool;

/** @brief Construct a PostProcessPassProperties holding the property type for a pass. */
auto MakeDefaultPassProperties(PostProcessPassFlag::type pass) -> PostProcessPassProperties;
} // namespace nc
