#pragma once

#include "utility/NcError.h"

#include <concepts>
#include <fstream>
#include <string>
#include <string_view>

namespace nc::config::internal
{
const auto INI_SKIP_CHARS = std::string{";#["};
const auto INI_KEY_VALUE_DELIM = '=';

inline bool ParseLine(const std::string& line, std::string& key, std::string& value)
{
    if (INI_SKIP_CHARS.find(line[0]) != INI_SKIP_CHARS.npos)
        return false;

    auto delimIndex = line.find(INI_KEY_VALUE_DELIM);
    if (delimIndex == line.npos)
        return false;

    key = line.substr(0, delimIndex);
    value = line.substr(delimIndex + 1);
    return true;
}

template<class Config_t, std::invocable<std::string, std::string, Config_t*> MapKeyValueFunc_t>
void Read(std::string_view filePath, MapKeyValueFunc_t mapFunc, Config_t* out)
{
    std::ifstream inFile;
    inFile.open(filePath.data());
    if(!inFile.is_open())
        throw NcError(std::string{"Failure opening: "} + filePath.data());

    std::string line{}, key{}, value{};
    while(!inFile.eof())
    {
        if(inFile.fail())
            throw NcError(std::string{"Stream failure while reading config: "} + filePath.data());

        std::getline(inFile, line, '\n');
        if(ParseLine(line, key, value))
            mapFunc(key, value, out);
    }

    inFile.close();
}
} // namespace nc::config::internal
