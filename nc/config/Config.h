#pragma once
#include <string>

namespace nc::config
{
    struct Project
    {
        bool Validate() const;
        std::string projectName;
    };

    struct Graphics
    {
        bool Validate() const;
        unsigned int screenWidth;
        unsigned int screenHeight;
        unsigned int targetFPS;
        double frameUpdateInterval;
        std::string shadersPath;
    };
    
    struct Physics
    {
        bool Validate() const;
        double fixedUpdateInterval;
    };

    struct Config
    {
        Project project;
        Graphics graphics;
        Physics physics;
    };

    namespace detail
    {
        struct ConfigPaths
        {
            const std::string projectConfigPath;
            const std::string graphicsConfigPath;
            const std::string physicsConfigPath;
        };

        Config Read(const ConfigPaths& paths) noexcept(false);
    }
}