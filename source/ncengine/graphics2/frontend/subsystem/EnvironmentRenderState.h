#pragma once

#include "ncengine/asset/AssetViews.h"

namespace nc::graphics
{
struct EnvironmentRenderState
{
    Vector4 primaryColor = Vector4::One();
    Vector4 secondaryColor = Vector4::One();
    Vector4 tertiaryColor = Vector4::One();
    uint32_t skyboxIndex = 0u;
    uint32_t useSkybox = 0u;
    uint32_t useColorOverride = 0u;
};
} // namespace nc::graphics
