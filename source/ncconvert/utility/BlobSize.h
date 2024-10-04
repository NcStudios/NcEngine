#pragma once

#include "ncasset/AssetsFwd.h"

#include <cstddef>

namespace nc::convert
{
/** @brief Get the serialized size in bytes for an AudioClip. */
auto GetBlobSize(const asset::AudioClip& asset) -> size_t;

/** @brief Get the serialized size in bytes for a ConcaveCollider. */
auto GetBlobSize(const asset::ConcaveCollider& asset) -> size_t;

/** @brief Get the serialized size in bytes for a CubeMap. */
auto GetBlobSize(const asset::CubeMap& asset) -> size_t;

/** @brief Get the serialized size in bytes for a HullCollider. */
auto GetBlobSize(const asset::HullCollider& asset) -> size_t;

/** @brief Get the serialized size in bytes for a Mesh. */
auto GetBlobSize(const asset::Mesh& asset) -> size_t;

/** @brief Get the serialized size in bytes for a SkeletalAnimation. */
auto GetBlobSize(const asset::SkeletalAnimation& asset) -> size_t;

/** @brief Get the serialized size in bytes for a Texture. */
auto GetBlobSize(const asset::Texture& asset) -> size_t;
} // namespace nc::convert
