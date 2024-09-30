#pragma once

#include "ncasset/AssetsFwd.h"

#include <cstdint>
#include <iosfwd>

namespace nc::convert
{
/** @brief Write an AudioClip to a binary stream. */
void Serialize(std::ostream& stream, const asset::AudioClip& data, uint64_t version);

/** @brief Write a ConcaveCollider to a binary stream. */
void Serialize(std::ostream& stream, const asset::ConcaveCollider& data, uint64_t version);

/** @brief Write a CubeMap to a binary stream. */
void Serialize(std::ostream& stream, const asset::CubeMap& data, uint64_t version);

/** @brief Write a HullCollider to a binary stream. */
void Serialize(std::ostream& stream, const asset::HullCollider& data, uint64_t version);

/** @brief Write a Mesh to a binary stream. */
void Serialize(std::ostream& stream, const asset::Mesh& data, uint64_t version);

/** @brief Write a SkeletalAnimation to a binary stream. */
void Serialize(std::ostream& stream, const asset::SkeletalAnimation& data, uint64_t version);

/** @brief Write a Texture to a binary stream. */
void Serialize(std::ostream& stream, const asset::Texture& data, uint64_t version);
} // nc::convert
