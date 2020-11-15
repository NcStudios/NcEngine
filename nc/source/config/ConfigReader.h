#pragma once

#include <fstream>
#include <string>

namespace
{
    const auto INI_SKIP_CHARS = std::string{";#["};
    const auto INI_KEY_VALUE_DELIM = '=';

    bool ParseLine(const std::string& line, std::string& key, std::string& value)
    {
        if (INI_SKIP_CHARS.find(line[0]) != INI_SKIP_CHARS.npos)
        {
            return false;
        }

        auto delimIndex = line.find(INI_KEY_VALUE_DELIM);
        if (delimIndex == line.npos)
        {
            return false;
        }

        key = line.substr(0, delimIndex);
        value = line.substr(delimIndex + 1);
        return true;
    }
}

namespace nc::config
{
    template<class Config_t,
             class MapKeyValueFunc_t,
             class = std::enable_if_t<std::is_invocable_v<MapKeyValueFunc_t, std::string, std::string, Config_t&>>>
    void Read(std::string filePath, MapKeyValueFunc_t mapFunc, Config_t& out)
    {
        std::ifstream inFile;
        inFile.open(filePath);
        if(!inFile.is_open())
        {
            throw std::runtime_error("Failed to load config file.");
        }

        std::string line{}, key{}, value{};
        while(!inFile.eof())
        {
            if(inFile.fail())
            {
                throw std::runtime_error("Stream failure while reading config");
            }

            std::getline(inFile, line, '\n');
            if(ParseLine(line, key, value))
            {
                mapFunc(key, value, out);
            }
        }

        inFile.close();
    }
}