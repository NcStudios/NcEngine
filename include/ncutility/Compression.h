/**
 * @file Compression.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace nc
{
/** @brief Option for influencing the compression ratio and speed of nc::Compress(). */
enum class CompressionLevel : uint8_t
{
    Default,
    Fast,
    Max
};

/** @brief The maximum size in bytes of input that can be provided to nc::Compress(). */
static constexpr auto compressMaxInputSize = size_t{2113929216};

/**
 * @brief Compress a range of bytes using LZ4/LZ4HC.
 * @param src The data to compress. Must not exceed compressMaxInputSize.
 * @param level The compression level to apply.
 * @return The compressed data as a vector of bytes.
 * @throw NcError is thrown on invalid parameters.
 */
auto Compress(std::span<const char> src, CompressionLevel level = CompressionLevel::Default) -> std::vector<char>;

/**
 * @brief Decompress a range of bytes compressed with LZ4/LZ4HC.
 * @param src The data to decompress.
 * @param maxDecompressedSize Size upper bound of the decompressed data.
 * @return The decompressed data as a vector of bytes.
 * @throw NcError is thrown if src is malformed or the specified max size is insufficient.
 */
auto Decompress(std::span<const char> src, size_t maxDecompressedSize) -> std::vector<char>;
} // namespace nc
