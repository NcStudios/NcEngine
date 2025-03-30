#include "Image.h"
#include "utility/EnumExtensions.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#define STB_DXT_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_dxt.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize2.h"

#include <utility>

namespace
{
auto ToVector(const nc::convert::RawImage& image, uint32_t numBytes) -> std::vector<unsigned char>
{
    auto out = std::vector<unsigned char>(numBytes);
    std::memcpy(out.data(), image.get(), numBytes);
    return out;
}

auto GetPaddedDimension(int dimension) -> int
{
    return (dimension + 3) & ~3;
}
} // anonymous namespace

namespace nc::convert
{
auto MakeRawImage(int width,
                  int height,
                  const unsigned char fillRGBA[4]) -> RawImage
{
    const auto numBytes = width * height * 4;
    auto bytes = (unsigned char*)std::malloc(numBytes);
    for (auto i = 0; i < numBytes; i += 4)
    {
        std::memcpy(bytes + i, fillRGBA, 4);
    }

    return RawImage{bytes};
}

Image::Image(const std::filesystem::path& path)
{
    const auto pathString = path.string(); // because path.c_str() is wchar* on Windows
    auto nChannels = int32_t{};
    auto ptr = stbi_load(pathString.c_str(), &m_width, &m_height, &nChannels, STBI_rgb_alpha);
    if(!ptr)
    {
        throw NcError("Failed to load texture file: ", pathString);
    }

    m_data = RawImage{ptr};
}

auto Image::MakeTextureSubResource() const -> asset::TextureSubResource
{
    return asset::TextureSubResource{
        .width = static_cast<uint32_t>(m_width),
        .height = static_cast<uint32_t>(m_height),
        .pixelData = ToVector(m_data, GetSizeInBytes())
    };
}

void Image::ResizePadded()
{
    if (!RequiresCompressionPadding())
    {
        return;
    }

    const auto paddedWidth = GetPaddedDimension(m_width);
    const auto paddedHeight = GetPaddedDimension(m_height);
    auto paddedImage = MakeRawImage(paddedWidth, paddedHeight, paddingRGBA);

    auto dst = paddedImage.get();
    const auto src = m_data.get();
    const auto dstScanLineSize = paddedWidth * numChannels;
    const auto srcScanLineSize = m_width * numChannels;
    for (auto scanLine = 0; scanLine < m_height; ++scanLine)
    {
        const auto dstLine = dst + scanLine * dstScanLineSize;
        const auto srcLine = src + scanLine * srcScanLineSize;
        std::memcpy(dstLine, srcLine, srcScanLineSize);
    }

    m_data = std::move(paddedImage);
    m_width = paddedWidth;
    m_height = paddedHeight;
}

auto Image::Compress(asset::TextureFormat format) const -> asset::TextureSubResource
{
    NC_ASSERT(
        IsCompressedTextureFormat(format) && !RequiresCompressionPadding(),
        fmt::format(
            "Invalid parameters for compression. Resize image or use an appropriate format.\n"
            "\tTextureFormat '{}', Width '{}', Height '{}'",
            std::to_underlying(format),
            m_width,
            m_height
        )
    );

    constexpr auto blockSize = 16;
    const auto useAlpha = TextureFormatHasAlpha(format);
    const auto numBlocks = (m_width / 4) * (m_height / 4);
    auto compressedData = std::vector<unsigned char>(numBlocks * blockSize);
    unsigned char block[blockSize * 4];

    // compress in 4x4 RGBA blocks
    const auto src = m_data.get();
    for (auto y = 0; y < m_height; y += 4)
    {
        for (auto x = 0; x < m_width; x += 4)
        {
            for (auto j = 0; j < 4; ++j)
            {
                for (auto i = 0; i < 4; ++i)
                {
                    const auto srcX = x + i;
                    const auto srcY = y + j;
                    const auto srcIndex = (srcY * m_width + srcX) * 4;
                    const auto dstIndex = (j * 4 + i) * 4;
                    std::memcpy(block + dstIndex, src + srcIndex, 4);
                }
            }

            const auto blockIndex = ((y / 4) * (m_width / 4) + (x / 4)) * blockSize;
            stb_compress_dxt_block(&compressedData[blockIndex], block, useAlpha, STB_DXT_HIGHQUAL);
        }
    }

    return asset::TextureSubResource{
        .width = static_cast<uint32_t>(m_width),
        .height = static_cast<uint32_t>(m_height),
        .pixelData = std::move(compressedData)
    };
}

void Image::Resize(int width, int height)
{
    auto* data = stbir_resize_uint8_srgb(
        m_data.get(),
        m_width,
        m_height,
        0,
        nullptr,
        width,
        height,
        0,
        STBIR_RGBA
    );

    if (!data)
    {
        throw NcError{"Image resize failed"};
    }

    m_data = RawImage{data};
    m_width = width;
    m_height = height;
}
} // namespace nc::convert
