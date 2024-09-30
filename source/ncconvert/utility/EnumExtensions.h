#pragma once

#include "ncasset/AssetType.h"

#include <string>

namespace nc::convert
{
auto CanOutputMany(asset::AssetType type) -> bool;
auto ToAssetType(std::string type) -> asset::AssetType;
auto ToString(asset::AssetType type) -> std::string;
}
