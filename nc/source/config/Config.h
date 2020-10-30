#pragma once
#include <string>

namespace nc::config
{
    struct Player
    {
        bool Validate() const;
        std::string playerName;
    };

    struct Project
    {
        bool Validate() const;
        std::string projectName;
        std::string playerName;
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
        Player player;
        Project project;
        Graphics graphics;
        Physics physics;
    };

    namespace detail
    {
        struct ConfigPaths
        {
            const std::string playerConfigPath;
            const std::string projectConfigPath;
            const std::string graphicsConfigPath;
            const std::string physicsConfigPath;
        };

        Config Read(const ConfigPaths& paths) noexcept(false);
    }
}