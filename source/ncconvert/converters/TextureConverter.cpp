#include "TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/Path.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

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

    const auto pathString = path.string(); // because path.c_str() is wchar* on Windows
    auto width = int32_t{};
    auto height = int32_t {};
    auto nChannels = int32_t{};
    auto* rawPixels = stbi_load(pathString.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
    if(!rawPixels)
    {
        throw NcError("Failed to load texture file: ", pathString);
    }

    auto pixels = std::vector<unsigned char>{};
    const auto nBytes = width * height * asset::Texture::numChannels; // ignore nChannels because we force to 4 8-bit channels
    pixels.reserve(nBytes);
    std::copy(rawPixels, rawPixels + nBytes, std::back_inserter(pixels));
    ::free(rawPixels);

    return nc::asset::Texture{
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        std::move(pixels)
    };
}
} // namespace nc::covnert
