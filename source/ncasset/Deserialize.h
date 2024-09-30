#pragma once

#include "ncasset/NcaHeader.h"
#include "ncasset/AssetsFwd.h"
#include "ncasset/AssetType.h"

#include <iosfwd>

namespace nc::asset
{
/** @brief A header and asset pair returned from a deserialize operation. */
template<class AssetType>
struct DeserializedResult
{
    NcaHeader header;
    AssetType asset;
};

/** @brief Read an NcaHeader from a binary stream. */
auto DeserializeHeader(std::istream& stream) -> NcaHeader;

/** @brief Construct an AudioClip from data in a binary stream. */
auto DeserializeAudioClip(std::istream& stream) -> DeserializedResult<AudioClip>;

/** @brief Construct a ConcaveCollider from data in a binary stream. */
auto DeserializeConcaveCollider(std::istream& stream) -> DeserializedResult<ConcaveCollider>;

/** @brief Construct a CubeMap from data in a binary stream. */
auto DeserializeCubeMap(std::istream& stream) -> DeserializedResult<CubeMap>;

/** @brief Construct a HullCollider from data in a binary stream. */
auto DeserializeHullCollider(std::istream& stream) -> DeserializedResult<HullCollider>;

/** @brief Construct a Mesh from data in a binary stream. */
auto DeserializeMesh(std::istream& stream) -> DeserializedResult<Mesh>;

/** @brief Construct a SkeletalAnimation from data in a binary stream. */
auto DeserializeSkeletalAnimation(std::istream& stream) -> DeserializedResult<SkeletalAnimation>;

/** @brief Construct a Texture from data in a binary stream. */
auto DeserializeTexture(std::istream& stream) -> DeserializedResult<Texture>;
} // nc::asset
