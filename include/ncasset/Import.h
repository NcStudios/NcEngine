/**
 * @file Import.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Assets.h"
#include "NcaHeader.h"

#include <filesystem>
#include <iosfwd>

namespace nc::asset
{
/** @brief Read an AudioClip asset from an .nca file. */
auto ImportAudioClip(const std::filesystem::path& ncaPath) -> AudioClip;

/** @brief Read an AudioClip asset from a binary stream. */
auto ImportAudioClip(std::istream& data) -> AudioClip;

/** @brief Read a ConcaveCollider asset from an .nca file. */
auto ImportConcaveCollider(const std::filesystem::path& ncaPath) -> ConcaveCollider;

/** @brief Read a ConcaveCollider asset from a binary stream. */
auto ImportConcaveCollider(std::istream& data) -> ConcaveCollider;

/** @brief Read a CubeMap asset from an .nca file. */
auto ImportCubeMap(const std::filesystem::path& ncaPath) -> CubeMap;

/** @brief Read a CubeMap asset from a binary stream. */
auto ImportCubeMap(std::istream& data) -> CubeMap;

/** @brief Read a HullCollider asset from an .nca file. */
auto ImportHullCollider(const std::filesystem::path& ncaPath) -> HullCollider;

/** @brief Read a HullCollider asset from a binary stream. */
auto ImportHullCollider(std::istream& data) -> HullCollider;

/** @brief Read a Mesh asset from an .nca file. */
auto ImportMesh(const std::filesystem::path& ncaPath) -> Mesh;

/** @brief Read a Mesh asset from a binary stream. */
auto ImportMesh(std::istream& data) -> Mesh;

/** @brief Read a SkeletalAnimation asset from an .nca file. */
auto ImportSkeletalAnimation(const std::filesystem::path& ncaPath) -> SkeletalAnimation;

/** @brief Read a SkeletalAnimation asset from a binary stream. */
auto ImportSkeletalAnimation(std::istream& data) -> SkeletalAnimation;

/** @brief Read a Texture asset from an .nca file. */
auto ImportTexture(const std::filesystem::path& ncaPath) -> Texture;

/** @brief Read a Texture asset from a binary stream */
auto ImportTexture(std::istream& data) -> Texture;

/** @brief Read the header from an .nca file. */
auto ImportNcaHeader(const std::filesystem::path& ncaPath) -> NcaHeader;

/** @brief Read the header from an asset in a binary stream. */
auto ImportNcaHeader(std::istream& data) -> NcaHeader;
} // namespace nc::asset
