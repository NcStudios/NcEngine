#pragma once

#include "ncasset/AssetType.h"

#include <string_view>
#include <string>

namespace nc::convert
{
auto CanOutputMany(asset::AssetType type) -> bool;
auto ToAssetType(std::string type) -> asset::AssetType;
auto ToString(asset::AssetType type) -> std::string;

auto IsCompressedTextureFormat(asset::TextureFormat format) -> bool;
auto TextureFormatHasAlpha(asset::TextureFormat format) -> bool;
auto GetMinimumDimension(asset::TextureFormat format) -> uint32_t;
auto ToString(asset::TextureFormat format) -> std::string_view;
}
