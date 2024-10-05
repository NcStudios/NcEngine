#include "ncutility/Compression.h"
#include "ncutility/NcError.h"

#include "lz4/lz4.h"
#include "lz4/lz4hc.h"

namespace nc
{
auto Compress(std::span<const char> src, CompressionLevel level) -> std::vector<char>
{
    NC_ASSERT(src.size() <= compressMaxInputSize, "Compression source data exceeds max size.");
    const auto srcSize = static_cast<int>(src.size());
    const auto dstCapacity = ::LZ4_compressBound(srcSize);
    auto dst = std::vector<char>(static_cast<size_t>(dstCapacity), '\0');
    const auto bytesWritten = [&]()
    {
        switch(level)
        {
            // The mapping here is a little awkward. We're not very concerned with compression speed,
            // so we choose 'Default' to mean high compression mode and 'Fast' to mean default mode.
            case nc::CompressionLevel::Default:
                return ::LZ4_compress_HC(src.data(), dst.data(), srcSize, dstCapacity, LZ4HC_CLEVEL_DEFAULT);
            case nc::CompressionLevel::Fast:
                return ::LZ4_compress_default(src.data(), dst.data(), srcSize, dstCapacity);
            case nc::CompressionLevel::Max:
                return ::LZ4_compress_HC(src.data(), dst.data(), srcSize, dstCapacity, LZ4HC_CLEVEL_MAX);
        }

        throw nc::NcError{fmt::format("Unknown compression level '{}'.", static_cast<unsigned>(level))};
    }();

    NC_ASSERT(bytesWritten > 0, "Unexpected compression failure."); // should be impossible
    dst.resize(static_cast<size_t>(bytesWritten));
    dst.shrink_to_fit();
    return dst;
}

auto Decompress(std::span<const char> src, size_t maxDecompressedSize) -> std::vector<char>
{
    const auto srcSize = static_cast<int>(src.size());
    const auto dstCapacity = static_cast<int>(maxDecompressedSize);
    auto dst = std::vector<char>(maxDecompressedSize, '\0');
    const auto result = ::LZ4_decompress_safe(src.data(), dst.data(), srcSize, dstCapacity);
    if (result < 0)
    {
        throw NcError(fmt::format("Decompression failed with error '{}'", result));
    }

    dst.resize(static_cast<size_t>(result)); // On success, result == numBytesRead
    dst.shrink_to_fit();
    return dst;
}
} // namespace nc
