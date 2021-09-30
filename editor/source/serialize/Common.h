#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace nc::editor
{
    const auto HeaderExtension = std::string{".h"};
    const auto SourceExtension = std::string{".cpp"};
    const auto GeneratedSourceExtension = std::string{".gen"};

    inline std::string StripCharacter(std::string value, char target)
    {
        value.erase(std::remove(value.begin(), value.end(), target), value.end());
        return value;
    }
}