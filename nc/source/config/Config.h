#pragma once
#include <string>

namespace nc::config
{
    struct Config
    {
        struct
        {
            std::string userName;
        } user;

        struct
        {
            std::string projectName;
            std::string logFilePath;
        } project;
        
        struct
        {
            unsigned int screenWidth;
            unsigned int screenHeight;
            unsigned int targetFPS;
            double frameUpdateInterval;
            std::string shadersPath;
        } graphics;

        struct
        {
            double fixedUpdateInterval;
        } physics;
    };

    namespace detail
    {
        Config Load() noexcept(false);
        void Save(const nc::config::Config& config) noexcept(false);
    }
}