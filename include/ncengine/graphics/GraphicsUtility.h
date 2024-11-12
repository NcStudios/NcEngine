/**
 * @file GraphicsUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/graphics/Material.h"

#include <span>
#include <string_view>

namespace nc
{
/** @brief Returns a view of all material pass names, ordered by ascending flag value. */
auto GetMaterialPassNames() -> std::span<const std::string_view>;

/** @brief Returns a view of all material pass flags, ordered by ascending flag value. */
auto GetMaterialPassFlags() -> std::span<const MaterialPasses>;
} // namespace nc
