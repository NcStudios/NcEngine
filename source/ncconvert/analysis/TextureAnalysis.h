#pragma once

#include <cstdint>
#include <vector>

namespace nc
{
namespace asset
{
template<typename T>
struct Texture;
} // namespace asset

namespace convert
{
enum class CubeMapAtlasLayout
{
    HorizontalArray,
    VerticalArray,
    HorizontalCross,
    VerticalCross
};

struct SubTexturePos
{
    uint32_t x;
    uint32_t y;
};

struct CubeMapAtlasSubTextureInfo
{
    uint32_t sideLength;
    SubTexturePos frontPosition;
    SubTexturePos backPosition;
    SubTexturePos topPosition;
    SubTexturePos bottomPosition;
    SubTexturePos rightPosition;
    SubTexturePos leftPosition;
};

auto GetCubeMapAtlasLayout(const nc::asset::Texture<unsigned char>& texture) -> CubeMapAtlasLayout;
auto GetSubTextureInfo(const nc::asset::Texture<unsigned char>& texture) -> CubeMapAtlasSubTextureInfo;
auto GetMipLevels(uint32_t width, uint32_t height, uint32_t minDimension = 1u) -> uint32_t;
} // namespace convert
} // namespace nc
