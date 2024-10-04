/**
 * @file NcaHeader.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "AssetType.h"

#include <cstdint>
#include <iosfwd>
#include <string_view>

namespace nc::asset
{
constexpr auto version4 = 4ull;
constexpr auto currentVersion = version4;

/** @brief Identifiers for asset blobs in .nca files. */
struct MagicNumber
{
    static constexpr auto audioClip = std::string_view{"CLIP"};
    static constexpr auto concaveCollider = std::string_view{"CONC"};
    static constexpr auto cubeMap = std::string_view{"CUBE"};
    static constexpr auto hullCollider = std::string_view{"HULL"};
    static constexpr auto mesh = std::string_view{"MESH"};
    static constexpr auto shader = std::string_view{"SHAD"};
    static constexpr auto skeletalAnimation = std::string_view{"SKEL"};
    static constexpr auto texture = std::string_view{"TEXT"};
};

/** @brief Common file header for all asset types. */
struct NcaHeader
{
    /**
     * @brief Size of a serialized NcaHeader.
     * @note Binary size does not include null terminators.
     */
    static constexpr auto binarySize = size_t{24};

    /** @brief Asset type identifier. */
    char magicNumber[5] = "NONE";

    /** @brief Compression type for the asset blob. */
    char compressionAlgorithm[5] = "NONE";

    /** @brief The Fnv1a hash of the asset's friendly name. */
    uint64_t version = currentVersion;

    /** @brief Size in bytes of the asset blob following this header. */
    uint64_t size = 0;
};

/** @brief Get the AssetType for an NcaHeader. */
auto GetAssetType(const NcaHeader& header) -> AssetType;

/** @brief Check if deserialization is supported for an Nca version. */
auto IsVersionSupported(uint64_t version) noexcept -> bool;

/** @brief Serialize an NcaHeader to a stream. */
void Serialize(std::ostream& stream, const NcaHeader& header);

/** @brief Deserialize an NcaHeader from a stream. */
void Deserialize(std::istream& stream, NcaHeader& header);
} // namespace nc::asset
