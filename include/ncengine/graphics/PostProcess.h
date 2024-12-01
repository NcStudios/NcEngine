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
struct PostProcessPass
{
    using type = uint64_t;

    static constexpr auto None    = type{0};
    static constexpr auto Alpha   = type{1 << 0};
    static constexpr auto Depth   = type{1 << 1};
    static constexpr auto Normals = type{1 << 2};
    static constexpr auto Outline = type{1 << 3};
};

/** @brief Identifier for a post process effect. */
using PostProcessEffectId = uint32_t;

/** @brief Set of flags indicating the passes used by a post process effect. */
using PostProcessEffectPasses = PostProcessPass::type;

/** @brief Null identifier for a post process effect. */
constexpr auto NullPostProcessEffectId = std::numeric_limits<PostProcessEffectId>::max();

/** @brief Identifier for the moebius post process effect. */
constexpr auto MoebiusEffectId = PostProcessEffectId{0};

/** @brief Pass flags for the moebius post process effect. */
constexpr auto MoebiusEffectPasses = PostProcessPass::Alpha   |
                                     PostProcessPass::Depth   |
                                     PostProcessPass::Normals |
                                     PostProcessPass::Outline;

/** @brief Post process property type representing an empty or uninitialized state. */
struct EmptyPassProperties {};

/** @brief Properties for the outline pass. */
struct OutlinePassProperties
{
    Vector3 color = Vector3::Zero();
    float width = 1.0f;
};

/** @brief Generic post process property type. */
using PostProcessPassProperties = std::variant<EmptyPassProperties,
                                               OutlinePassProperties>;

/** @brief Returns if a post process pass has a property type. */
auto PassHasProperties(PostProcessPass::type pass) -> bool;

/** @brief Construct a PostProcessPassProperties holding the property type for a pass. */
auto MakeDefaultPassProperties(PostProcessPass::type pass) -> PostProcessPassProperties;
} // namespace nc
