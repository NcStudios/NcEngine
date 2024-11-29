/**
 * @file PostProcess.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"

#include <variant>

namespace nc
{
struct PostProcessPass
{
    using type = uint64_t;

    static constexpr auto None    = type{0};
    static constexpr auto Alpha   = type{1 << 0};
    static constexpr auto Depth   = type{1 << 1};
    static constexpr auto Normals = type{1 << 2};
    static constexpr auto Outline = type{1 << 3};
};

using PostProcessEffectId = uint32_t;
using PostProcessEffectPasses = PostProcessPass::type;

constexpr auto NullPostProcessEffectId = std::numeric_limits<PostProcessEffectId>::max();

constexpr auto MoebiusEffect = PostProcessEffectId{0};
constexpr auto MoebiusEffectPasses = PostProcessPass::Alpha   |
                                     PostProcessPass::Depth   |
                                     PostProcessPass::Normals |
                                     PostProcessPass::Outline;

// might want empty state... not sure
struct EmptyPassProperties {};

struct OutlinePassProperties
{
    Vector3 color = Vector3::Zero();
    float width = 1.0f;
};

using PostProcessPassProperties = std::variant<EmptyPassProperties,
                                               OutlinePassProperties>;
} // namespace nc
