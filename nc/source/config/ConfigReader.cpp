#include "ConfigReader.h"

namespace nc::config
{
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