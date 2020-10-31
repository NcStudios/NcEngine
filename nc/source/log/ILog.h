#pragma once

#include <string>

namespace nc::log
{
    class ILog
    {
        public:
            virtual void AddItem(std::string item) = 0;
    };
}