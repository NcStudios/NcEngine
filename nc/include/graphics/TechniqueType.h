#pragma once

#include <cstdint>

namespace nc::graphics
{
    enum class TechniqueType : uint8_t
    {
        PhongShading = 0,
        Wireframe = 1,
        Particle = 2
    };
}
