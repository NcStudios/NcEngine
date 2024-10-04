#pragma once

#include <cstdint>
#include <vector>

namespace nc
{
namespace asset
{
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

auto GetCubeMapAtlasLayout(const nc::asset::Texture& texture) -> CubeMapAtlasLayout;
auto GetSubTextureInfo(const nc::asset::Texture& texture) -> CubeMapAtlasSubTextureInfo;
} // namespace convert
} // namespace nc
