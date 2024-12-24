#pragma once

#include "graphics2/ShaderTypes.h"

#include <span>

namespace nc::graphics
{
struct LightRenderState
{
    std::span<const LightData> lights;
};
} // namespace nc::graphics
