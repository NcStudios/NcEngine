#pragma once

#include <string>

namespace nc::graphics::vulkan
{
    struct PhongMaterial
    {
        std::string baseColor; // Slot 0
        std::string normal; // Slot 1
        std::string roughness; // Slot 2
    };
}