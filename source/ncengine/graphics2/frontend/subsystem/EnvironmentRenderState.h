#pragma once

#include "ncengine/asset/AssetViews.h"

namespace nc::graphics
{
struct EnvironmentRenderState
{
    uint32_t skyboxIndex = 0u;
    uint32_t useSkybox = 0u;
};
} // namespace nc::graphics
