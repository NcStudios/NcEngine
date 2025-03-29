#include "TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/Path.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_DXT_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_resize2.h"
#include "stb/stb_dxt.h"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace
{
const auto supportedFileExtensions = std::array<std::string, 4>{".png", ".jpg", ".jpeg", ".bmp"};

auto ReadTextureFromAtlas(const nc::asset::Texture& atlas, unsigned char* dest, nc::convert::SubTexturePos pos, uint32_t sideLength) -> size_t
{
    const auto source = atlas.pixelData.data();
    const auto xByteOffset = pos.x * nc::asset::Texture::numChannels;
    const auto scanLineByteSize = atlas.width * nc::asset::Texture::numChannels;
    const auto readSize = sideLength * nc::asset::Texture::numChannels;
    auto bytesRead = 0ull;
    for (auto scanLine = pos.y; scanLine < pos.y + sideLength; ++scanLine)
    {
        const auto offset = scanLine * scanLineByteSize + xByteOffset;
        std::memcpy(dest + bytesRead, source + offset, readSize);
        bytesRead += readSize;
    }

    return bytesRead;
}
} // anonymous namespace

namespace nc::convert
{
auto TextureConverter::ImportCubeMap(const std::filesystem::path& path) -> asset::CubeMap
{
    auto atlas = ImportTexture(path);
    const auto atlasInfo = GetSubTextureInfo(atlas);
    const auto sideLen = atlasInfo.sideLength;
    const auto nBytesPerFace = sideLen * sideLen * asset::Texture::numChannels;
    auto pixels = std::vector<unsigned char>(nBytesPerFace * 6u);
    auto curPos = 0ull;
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.frontPosition, sideLen);
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.backPosition, sideLen);
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.topPosition, sideLen);
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.bottomPosition, sideLen);
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.rightPosition, sideLen);
    curPos += ::ReadTextureFromAtlas(atlas, pixels.data() + curPos, atlasInfo.leftPosition, sideLen);

    return nc::asset::CubeMap{
        sideLen,
        std::move(pixels)
    };
}

auto TextureConverter::ImportTexture(const std::filesystem::path& path) -> asset::Texture
{
    if (!ValidateInputFileExtension(path, supportedFileExtensions))
    {
        throw NcError("Invalid input file: ", path.string());
    }

    auto image = Image{path};
    auto subresource = image.MakeTextureSubResource();
    return asset::Texture{
        subresource.width,
        subresource.height,
        std::move(subresource.pixelData)
    };
}


Image::Image(const std::filesystem::path& path)
{
    const auto pathString = path.string(); // because path.c_str() is wchar* on Windows
    auto nChannels = int32_t{};
    m_data = stbi_load(pathString.c_str(), &m_width, &m_height, &nChannels, STBI_rgb_alpha);
    if(!m_data)
    {
        throw NcError("Failed to load texture file: ", pathString);
    }
}

Image::~Image() noexcept
{
    if (m_data)
    {
        std::free(m_data);
    }
}

auto Image::GetSizeInBytes() const -> uint32_t
{
    return m_width * m_height * numChannels;
}

auto Image::MakeTextureSubResource() const -> asset::TextureSubResource
{
    const auto numBytes = GetSizeInBytes();
    auto pixels = std::vector<unsigned char>(numBytes);
    std::memcpy(pixels.data(), m_data, numBytes);
    return asset::TextureSubResource{
        .width = static_cast<uint32_t>(m_width),
        .height = static_cast<uint32_t>(m_height),
        .pixelData = std::move(pixels)
    };
}

// should be const
auto Image::Compress(asset::TextureFormat format,
                     asset::CompressionQuality quality) const -> asset::TextureSubResource
{
    const auto hasAlpha = format == asset::TextureFormat::BC3;
    const auto mode = std::to_underlying(quality);

    if (m_width % 4 != 0 || m_height % 4 != 0) {
        // throw leaks/i guess crashes us, but...
        // also can we pad or something?... that might be bad in-game?

        // DON"T DO THIS: we're returning a copy...

        // if (m_width >= 4 || m_height >= 4)
        {
            throw nc::NcError{"invalid assumptions"};
        }

        // auto newSrc = (unsigned char*)std::malloc(4 * 4 * 4);
        // // std::memset(newSrc, 0, 4 * 4 * 4);
        // std::memcpy(newSrc, m_data, m_width * m_height * 4);
        // // std::free(src);
        // m_data = newSrc;
        // m_width = 4;
        // m_height = 4;
    }

    constexpr auto blockSize = 16;
    const auto numBlocks = (m_width / 4) * (m_height / 4);
    auto compressedData = std::vector<unsigned char>(numBlocks * blockSize);

    // 4x4 RGBA block
    unsigned char block[4 * 4 * 4];
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
                    std::memcpy(block + dstIndex, m_data + srcIndex, 4);
                }
            }

            const auto blockIndex = ((y / 4) * (m_width / 4) + (x / 4)) * blockSize;
            stb_compress_dxt_block(&compressedData[blockIndex], block, hasAlpha, mode);
        }
    }

    return asset::TextureSubResource{
        .width = static_cast<uint32_t>(m_width),
        .height = static_cast<uint32_t>(m_height),
        .pixelData = std::move(compressedData)
    };
}
} // namespace nc::covnert
