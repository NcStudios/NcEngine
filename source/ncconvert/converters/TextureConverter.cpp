#include "TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/Path.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_DXT_IMPLEMENTATION
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

auto CompressBC3(const std::string&, unsigned char* src, int width, int height, int) -> nc::asset::Texture
{
    if (width % 4 != 0 || height % 4 != 0) {
        // throw leaks/i guess crashes us, but...
        // also can we pad or something?... that might be bad in-game?

        if (width >= 4 || height >= 4)
        {
            throw nc::NcError{"invalid assumptions"};
        }

        auto newSrc = (unsigned char*)std::malloc(4 * 4 * 4);
        // std::memset(newSrc, 0, 4 * 4 * 4);
        std::memcpy(newSrc, src, width * height * 4);
        // std::free(src);
        src = newSrc;
        width = 4;
        height = 4;

        // throw nc::NcError{fmt::format(
        //     "dimensions not divisible by 4: {}, {}x{}",
        //     name, width, height
        // )};
        // stbi_image_free(imageData);
    }

    // Allocate memory for the compressed data
    // size_t blockSize = 8; // BC1/DXT1 block size is 8 bytes
    size_t blockSize = 16;
    size_t numBlocks = (width / 4) * (height / 4);
    std::vector<unsigned char> compressedData(numBlocks * blockSize);

    // Compress each 4x4 block
    unsigned char* rgbaBlock = new unsigned char[16 * 4]; // Temporary storage for a 4x4 RGBA block
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            // Extract a 4x4 block from the image
            for (int j = 0; j < 4; ++j) {
                for (int i = 0; i < 4; ++i) {
                    int srcX = x + i;
                    int srcY = y + j;
                    int srcIndex = (srcY * width + srcX) * 4;

                    int dstIndex = (j * 4 + i) * 4;
                    memcpy(rgbaBlock + dstIndex, src + srcIndex, 4); // Copy RGBA
                }
            }

            // Compress the block to BC1 (DXT1)
            size_t blockIndex = ((y / 4) * (width / 4) + (x / 4)) * blockSize;
            stb_compress_dxt_block(&compressedData[blockIndex], rgbaBlock, 1, STB_DXT_NORMAL);
        }
    }

    return nc::asset::Texture{
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        std::move(compressedData)
    };
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

    auto texture = CompressBC3(path.string(), rawPixels, width, height, asset::Texture::numChannels);
    ::free(rawPixels);
    return texture;

    // auto pixels = std::vector<unsigned char>{};
    // const auto nBytes = width * height * asset::Texture::numChannels; // ignore nChannels because we force to 4 8-bit channels
    // pixels.reserve(nBytes);
    // std::copy(rawPixels, rawPixels + nBytes, std::back_inserter(pixels));
    // ::free(rawPixels);

    // return nc::asset::Texture{
    //     static_cast<uint32_t>(width),
    //     static_cast<uint32_t>(height),
    //     std::move(pixels)
    // };
}
} // namespace nc::covnert
