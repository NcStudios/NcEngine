/**
 * @file GraphicsUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"

#include <span>
#include <string_view>

namespace nc
{
/** @brief Returns a view of all material pass names, ordered by ascending flag value. */
auto GetMaterialPassNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all material pass flags, ordered by ascending flag value. */
auto GetMaterialPassFlags() -> std::span<const MaterialPassFlag::type>;

/**
 * @brief Returns a view of all currently implemented material pass flags.
 * @todo 794 Temporary solution while passes are being implemented. Add passes as they become available.
 *           Eventually, usage of this should switch to GetMaterialPassFlags().
 */
auto GetImplementedMaterialPassFlags() -> std::span<const MaterialPassFlag::type>;

/** @brief Returns a view of all post process pass names, ordered by ascending flag value. */
auto GetPostProcessPassNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all post process pass flags, ordered by ascending flag value. */
auto GetPostProcessPassFlags() -> std::span<const PostProcessPass::type>;

/** @brief Returns the name of a post process pass. */
auto GetPostProcessPassName(PostProcessPass::type pass) -> std::string_view;

/** @brief Returns a view of all post process effect names, ordered by ascending flag value. */
auto GetPostProcessEffectNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all post process effect ids, ordered by ascending id value. */
auto GetPostProcessEffectIds() -> std::span<const PostProcessEffectId>;

/** @brief Returns a view of the post process passes used by an effect, ordered by ascending flag value. */
auto GetPostProcessEffectPassFlags(PostProcessEffectId effectId) -> std::span<const PostProcessPass::type>;

/** @brief Returns the combined post process pass flags used by an effect. */
auto GetCombinedPostProcessEffectPassFlags(PostProcessEffectId effectId) -> PostProcessEffectPasses;
} // namespace nc
