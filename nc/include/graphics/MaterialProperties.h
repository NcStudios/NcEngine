#pragma once

#include "math/Vector3.h"

namespace nc::graphics
{
    struct MaterialProperties
    {
        Vector3 color = Vector3{0.25f, 0.25f, 0.25f};
        float specularIntensity = 0.6;
        float specularPower = 32.0f;
        float xTiling = 1.0f;
        float yTiling = 1.0f;
        float padding[1] = {0.0f};
    };
}