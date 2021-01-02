#pragma once

#include <cstdint>

namespace nc::graphics
{
    enum class TechniqueType : uint8_t
    {
        PhongShadingTechnique = 0,
        WireframeTechnique = 1
    };
}
