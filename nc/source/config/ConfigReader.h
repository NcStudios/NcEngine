#pragma once

#include <concepts>
#include <fstream>
#include <string>

namespace nc::config
{
    const auto INI_SKIP_CHARS = std::string{";#["};
    const auto INI_KEY_VALUE_DELIM = '=';

    bool ParseLine(const std::string& line, std::string& key, std::string& value);

    template<class Config_t, std::invocable<std::string, std::string, Config_t&> MapKeyValueFunc_t>
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