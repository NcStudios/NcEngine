#include "TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/EnumExtensions.h"
#include "utility/Image.h"
#include "utility/Log.h"
#include "utility/Path.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>

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

auto MakePrimaryTexture(nc::convert::Image& image,
                        nc::asset::TextureFormat format,
                        bool useCompression) -> nc::asset::Texture
{
    auto subresource = [&](){
        if (useCompression)
        {
            if (image.RequiresCompressionPadding())
            {
                image.ResizePadded();
                LOG("Invalid dimensions for compression. Image padded to '{}'x'{}'.", image.GetWidth(), image.GetHeight());
            }

            return image.Compress(format);
        }

        return image.MakeTextureSubResource();
    }();

    return nc::asset::Texture{
        .format = format,
        .width = subresource.width,
        .height = subresource.height,
        .pixelData = std::move(subresource.pixelData)
    };
}

auto GenerateMips(nc::convert::Image& image,
                  nc::asset::TextureFormat format,
                  bool useCompression) -> std::vector<nc::asset::TextureSubResource>
{
    const auto width = static_cast<uint32_t>(image.GetWidth());
    const auto height = static_cast<uint32_t>(image.GetHeight());
    const auto minDimension = nc::convert::GetMinimumDimension(format);
    const auto mipLevels = nc::convert::GetMipLevels(width, height, minDimension);
    NC_ASSERT(mipLevels >= 1, "Unexpected mipLevels");
    auto mipmaps = std::vector<nc::asset::TextureSubResource>{};
    mipmaps.reserve(mipLevels - 1);
    auto halfWidth = width;
    auto halfHeight = height;
    for (auto i = 1u; i < mipLevels; ++i)
    {
        halfWidth = std::max(halfWidth / 2, minDimension);
        halfHeight = std::max(halfHeight / 2, minDimension);
        image.Resize(halfWidth, halfHeight);
        mipmaps.push_back(
            useCompression
                ? image.Compress(format)
                : image.MakeTextureSubResource()
        );
    }

    return mipmaps;
}
} // anonymous namespace

namespace nc::convert
{
auto TextureConverter::ImportCubeMap(const std::filesystem::path& path) -> asset::CubeMap
{
    // TODO: what to do here?
    auto atlas = ImportTexture(path, asset::TextureFormat::RGBA8_UNORM_SRGB, false);
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

auto TextureConverter::ImportTexture(const std::filesystem::path& path,
                                     asset::TextureFormat format,
                                     bool generateMips) -> asset::Texture
{
    if (!ValidateInputFileExtension(path, supportedFileExtensions))
    {
        throw NcError("Invalid input file: ", path.string());
    }

    const auto useCompression = IsCompressedTextureFormat(format);
    auto image = Image{path};
    auto texture = MakePrimaryTexture(image, format, useCompression);
    if (generateMips)
    {
        texture.mipmaps = GenerateMips(image, format, useCompression);
    }

    return texture;
}
} // namespace nc::convert
