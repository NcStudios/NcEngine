#pragma once

#include <string_view>

namespace nc::config
{
    void LoadInternal(std::string_view configPath);
}