#pragma once

#include <string>

namespace project::config
{
    struct ProjectConfig
    {
        void Load();
        void Save();

        std::string userName;
    };
}