#pragma once

#include "graphics2/ShaderTypes.h"

#include <span>

namespace nc::graphics
{
struct LightRenderState
{
    std::span<const LightData> lights;
    std::span<const LightMatrixData> lightMatrices;
    float nearZ = 1.0f;
    float farZ = 40.0f;
};
} // namespace nc::graphics
