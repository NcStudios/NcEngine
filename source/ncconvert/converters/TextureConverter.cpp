#include "TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/EnumExtensions.h"
#include "utility/Image.h"
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

    auto texture = asset::Texture{
        .format = asset::TextureFormat::RGBA8_UNORM_SRGB, // TODO: take as param
        .width = subresource.width,
        .height = subresource.height,
        .pixelData = std::move(subresource.pixelData),
        .mipmaps = {
        },
    };

    const auto minDimension = GetMinimumDimension(texture.format);
    const auto mipLevels = GetMipLevels(texture.width, texture.height, minDimension);
    NC_ASSERT(mipLevels >= 1, "Unexpected mipLevels");
    texture.mipmaps.reserve(mipLevels - 1);
    auto halfWidth = texture.width;
    auto halfHeight = texture.height;
    for (auto i = 1u; i < mipLevels; ++i)
    {
        halfWidth = std::max(halfWidth / 2, minDimension);
        halfHeight = std::max(halfHeight / 2, minDimension);
        image.Resize(halfWidth, halfHeight);
        texture.mipmaps.push_back(image.MakeTextureSubResource());
    }

    return texture;
}
} // namespace nc::covnert
