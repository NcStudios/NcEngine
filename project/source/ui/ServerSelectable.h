#pragma once

#include <string>

namespace project::ui
{
    struct ServerSelectable
    {
        std::string name = "";
        std::string ip = "";
        bool isSelected = false;
    };
}