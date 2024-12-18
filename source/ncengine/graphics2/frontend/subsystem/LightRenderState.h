#pragma once

#include "graphics2/ShaderTypes.h"

#include <span>

namespace nc::graphics
{
struct LightRenderState
{
    std::span<const DirectionalLightData> directionalLights;
    std::span<const PointLightData> pointLights;
    std::span<const SpotLightData> spotLights;
};
} // namespace nc::graphics
