#include "Config.h"
#include "debug/NCException.h"

#include <fstream>
#include <limits>
#include <type_traits>

#include <iostream>


namespace nc::config
{

bool Project::Validate() const
{
    return projectName == "" ? false : true;
}

bool Graphics::Validate() const
{
    return true;
}

bool Physics::Validate() const
{
    return fixedUpdateInterval <= 0.0 ? false : true;
}

namespace detail
{
const std::string PROJECT_NAME_KEY{"project_name"};
const std::string SCREEN_WIDTH_KEY{"screen_width"};
const std::string SCREEN_HEIGHT_KEY{"screen_height"};
const std::string TARGET_FPS_KEY{"target_fps"};
const std::string FIXED_UPDATE_INTERVAL_KEY{"fixed_update_interval"};
const std::string INI_SKIP_CHARS{";#["};
const char INI_KEY_VALUE_DELIM = '=';

bool ParseLine(const std::string& line, std::string& key, std::string& value)
{
    if (INI_SKIP_CHARS.find(line[0]) != INI_SKIP_CHARS.npos)
    {
        return false;
    }

    auto delimIndex = line.find(INI_KEY_VALUE_DELIM);
    if (delimIndex == line.npos)
    {
        throw NcException("ParseLine - invalid syntax in config file");
    }

    key = line.substr(0, delimIndex);
    value = line.substr(delimIndex + 1);
    return true;
}


template<class Out,
         class KeyValueMapFunc,
         class = typename std::enable_if<std::is_invocable<KeyValueMapFunc, const std::string&, const std::string&, Out&>::value>::type>
Out ConfigReader(const std::string& path, KeyValueMapFunc& map)
{
    std::ifstream inFile;
    inFile.open(path);
    if(!inFile.is_open())
    {
        throw NcException("ConfigReader - failed to load file");
    }

    Out out{};
    std::string line{};
    std::string key{};
    std::string value{};
    while(!inFile.eof())
    {
        if(inFile.fail())
        {
            throw NcException("ConfigReader - ifstream failure");
        }

        std::getline(inFile, line, '\n');
        if (ParseLine(line, key, value))
        {
            map(key, value, out);
        }
    }

    inFile.close();
    
    if (!out.Validate())
    {
        throw NcException("ConfigReader - invalid config");
    }
    return out;
}

Project ReadProjectConfig(const std::string& path)
{
    auto mapFunc = [](const std::string& key, const std::string& value, Project& out)
    {
        if (key == PROJECT_NAME_KEY)
        {
            out.projectName = value;
        }
        else
        {
            throw nc::NcException("ConfigReader::ReadProjectConfig::mapFunc - unkown key");
        }
    };
    return ConfigReader<config::Project>(path, mapFunc);
}

Graphics ReadGraphicsConfig(const std::string& path)
{
    auto mapFunc = [](const std::string& key, const std::string& value, Graphics& out)
    {
        if (key == SCREEN_WIDTH_KEY)
        {
            out.screenWidth = std::stoi(value);
        }
        else if (key == SCREEN_HEIGHT_KEY)
        {
            out.screenHeight = std::stoi(value);
        }
        else if (key == TARGET_FPS_KEY)
        {
            out.targetFPS = std::stoi(value);
            out.frameUpdateInterval = 1.0 / static_cast<double>(out.targetFPS);
        }
        else
        {
            throw nc::NcException("ConfigReader::ReadGraphicsConfig::mapFunc - unkown key");
        }
    };
    return ConfigReader<config::Graphics>(path, mapFunc);
}

Physics ReadPhysicsConfig(const std::string& path)
{
    auto mapFunc = [](const std::string& key, const std::string& value, Physics& out)
    {
        if (key == FIXED_UPDATE_INTERVAL_KEY)
        {
            out.fixedUpdateInterval = std::stod(value);
        }
        else
        {
            throw nc::NcException("ConfigReader::ReadPhysicsConfig::mapFunc - unkown key");
        }
    };
    return ConfigReader<config::Physics>(path, mapFunc);
}

Config Read(const ConfigPaths& paths)
{
    return { ReadProjectConfig(paths.projectConfigPath),
             ReadGraphicsConfig(paths.graphicsConfigPath),
             ReadPhysicsConfig(paths.physicsConfigPath) };
}

} // end namespace detail 
} // end namespace nc::config