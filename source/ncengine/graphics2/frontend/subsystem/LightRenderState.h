#pragma once

#include "graphics2/ShaderTypes.h"

#include <span>

namespace nc::graphics
{
struct LightRenderState
{
    std::span<const LightData> lights;
    std::span<const LightMatrixData> lightMatrices;
    std::span<const CascadeData> cascades;
};
} // namespace nc::graphics
