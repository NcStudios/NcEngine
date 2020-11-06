#pragma once

#include <string>

namespace nc::log
{
    class IGameLog
    {
        public:
            virtual void Log(std::string item) = 0;
    };
}