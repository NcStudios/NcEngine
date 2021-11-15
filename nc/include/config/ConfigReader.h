#pragma once

#include "debug/Utils.h"

#include <concepts>
#include <fstream>
#include <string>

namespace nc::config
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
    void Read(std::string filePath, MapKeyValueFunc_t mapFunc, Config_t* out)
    {
        std::ifstream inFile;
        inFile.open(filePath);
        if(!inFile.is_open())
            throw NcError("Failure opening: " + filePath);

        std::string line{}, key{}, value{};
        while(!inFile.eof())
        {
            if(inFile.fail())
                throw NcError("Stream failure while reading config: " + filePath);

            std::getline(inFile, line, '\n');
            if(ParseLine(line, key, value))
                mapFunc(key, value, out);
        }

        inFile.close();
    }
}