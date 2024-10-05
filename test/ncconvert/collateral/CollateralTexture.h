#pragma once

#include "CollateralCommon.h"

#include <array>
#include <cstdint>

namespace collateral
{

constexpr auto red = uint32_t{0xFF0000FF};
constexpr auto green = uint32_t{0x00FF00FF};
constexpr auto blue = uint32_t{0x0000FFFF};
constexpr auto black = uint32_t{0x000000FF};
constexpr auto white = uint32_t{0xFFFFFFFF};
constexpr auto clear = uint32_t{0x00000000};

// Describes the collateral files named rgb_corners_4x8.XXX
namespace rgb_corners
{
const auto bmpFilePath = collateralDirectory / "rgb_corners_4x8.bmp";
const auto jpgFilePath = collateralDirectory / "rgb_corners_4x8.jpg";
const auto pngFilePath = collateralDirectory / "rgb_corners_4x8.png";
constexpr auto width = 4u;
constexpr auto height = 8u;
constexpr auto numPixels = width * height;
constexpr auto numBytes = numPixels * 4u;
constexpr auto pixels = std::array<uint32_t, 32>{
    red,   white, white, green,
    white, white, white, white,
    white, white, white, white,
    white, white, white, white,
    white, white, white, white,
    white, white, white, white,
    white, white, white, white,
    blue,  white, white, black
};
} // namespace rgb_corners

// Describes the collateral files named cube_map_XXX_XXX.png
namespace cube_map
{
const auto horizontalArrayFilePath = collateralDirectory / "cube_map_horizontal_array.png";
const auto verticalArrayFilePath = collateralDirectory / "cube_map_vertical_array.png";
const auto horizontalCrossFilePath = collateralDirectory / "cube_map_horizontal_cross.png";
const auto verticalCrossFilePath = collateralDirectory / "cube_map_vertical_cross.png";
constexpr auto faceSideLength = 2u;
constexpr auto numPixels = faceSideLength * faceSideLength * 6u;
constexpr auto numBytes = numPixels * 4u;
// This is the final CubeMap representation, not what is appears in the file
constexpr auto pixels = std::array<uint32_t, 24>{
    white, white, white, white,
    black, black, black, black,
    blue,  blue,  blue,  blue,
    clear, clear, clear, clear,
    red,   red,   red,   red,
    green, green, green, green
};
constexpr auto frontFacePixels = std::array<uint32_t, 4>{white, white, white, white};
constexpr auto backFacePixels = std::array<uint32_t, 4>{black, black, black, black};
constexpr auto topFacePixels = std::array<uint32_t, 4>{blue, blue, blue, blue};
constexpr auto bottomFacePixels = std::array<uint32_t, 4>{clear, clear, clear, clear};
constexpr auto rightFacePixels = std::array<uint32_t, 4>{red, red, red, red};
constexpr auto leftFacePixels = std::array<uint32_t, 4>{green, green, green, green};
} // namespace cube_map
} // namespace collateral
