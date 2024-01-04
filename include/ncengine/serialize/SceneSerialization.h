/**
 * @file SceneSerialization.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"

#include <iosfwd>

namespace nc
{
/** @brief Initial value in a binary scene fragment blob. */
constexpr auto g_sceneFragmentMagicNumber = 0x3ff0e17b;

/** @brief Version number serialized with a scene fragment. */
constexpr auto g_currentSceneFragmentVersion = 1u;

/** @brief Save current game state to a binary stream. */
void SaveSceneFragment(std::ostream& stream,
                       ecs::Ecs ecs,
                       const asset::AssetMap& assets,
                       std::function<bool(Entity)> entityFilter = nullptr);

/** @brief Load game state from a binary stream. */
void LoadSceneFragment(std::istream& stream,
                       ecs::Ecs ecs,
                       asset::NcAsset& assetModule);
} // namespace nc
