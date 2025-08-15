/**
 * @file GraphicsUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/graphics/Curve.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"

#include <span>
#include <string_view>

namespace nc
{
/** @brief Default aspect ratio. */
constexpr auto AspectRatio = 16.0f / 9.0f;

/** @brief Calculate the maximum region conforming to nc::AspectRatio that fits within dimensions. */
auto AdjustDimensionsToAspectRatio(const nc::Vector2& dimensions) -> nc::Vector2;

/** @brief Texture view types used throughout NcEngine. */
enum class TextureViewType : uint8_t
{
    None,
    ColorSink,
    DepthSink,
    UniShadowSink,
    PointShadowSink,
    Asset,
    PostProcess
};

/** @brief Pass flags that represent Miscellaneous passes */
struct MiscPassFlag
{
    using type = uint64_t;

    static constexpr auto Wireframe = type{1 << 0};
    static constexpr auto Particle  = type{1 << 1};
    static constexpr auto Skybox    = type{1 << 2};
};

/** @brief Returns a view of all material pass names, ordered by ascending flag value. */
auto GetMaterialPassNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all material pass flags, ordered by ascending flag value. */
auto GetMaterialPassFlags() -> std::span<const MaterialPassFlag::type>;

/** @brief Returns a view of all miscellaneous pass flags, ordered by ascending flag value. */
auto GetMiscsPassFlags() -> std::span<const MiscPassFlag::type>;

/** @brief Returns a view of all post process pass names, ordered by ascending flag value. */
auto GetPostProcessPassNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all post process pass flags, ordered by ascending flag value. */
auto GetPostProcessPassFlags() -> std::span<const PostProcessPassFlag::type>;

/** @brief Returns the name of a post process pass. */
auto GetPostProcessPassName(PostProcessPassFlag::type pass) -> std::string_view;

/** @brief Returns a view of all post process effect names, ordered by ascending flag value. */
auto GetPostProcessEffectNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all post process effect ids, ordered by ascending id value. */
auto GetPostProcessEffectIds() -> std::span<const PostProcessEffectId>;

/** @brief Returns a view of the post process passes used by an effect, ordered by ascending flag value. */
auto GetPostProcessEffectPassFlags(PostProcessEffectId effectId) -> std::span<const PostProcessPassFlag::type>;

/** @brief Returns the combined post process pass flags used by an effect. */
auto GetCombinedPostProcessEffectPassFlags(PostProcessEffectId effectId) -> PostProcessEffectPassFlags;

/** @brief Return a string name for a given CurveType. */
auto ToString(CurveType curve) -> std::string_view;

/** @brief Convert a string to a CurveType. */
auto ToCurveType(std::string_view curveName) -> CurveType;

/** @brief Returns a view of all CurveType names. */
auto GetCurveTypeNames() -> std::span<const std::string_view>;
} // namespace nc
