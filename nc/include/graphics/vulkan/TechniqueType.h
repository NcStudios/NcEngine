#pragma once

#include <cstdint>

namespace nc::graphics::vulkan
{
    enum class TechniqueType : uint8_t
    {
        None = 0,
        PhongAndUi = 1,
        Wireframe = 2
    };
}