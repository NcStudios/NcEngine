#pragma once

#include <string>

namespace nc::config
{
    void Load(const std::string& configPath);
    void Save();
    bool Validate();
}