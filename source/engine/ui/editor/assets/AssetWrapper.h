#pragma once

#include "ncasset/AssetType.h"

#include <string_view>
#include <vector>

namespace nc::ui::editor
{
auto GetLoadedAssets(asset::AssetType type) -> std::vector<std::string_view>;
} // namespace nc::ui::editor
