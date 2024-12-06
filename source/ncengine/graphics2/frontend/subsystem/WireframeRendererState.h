#pragma once

#include "graphics2/ShaderTypes.h"
#include "ncengine/asset/AssetViews.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
struct WireframeInstanceState
{
    WireframeData data;
    asset::MeshView mesh;
};

struct WireframeRendererRenderState
{
    std::vector<WireframeInstanceState> wireframeData;
};
} // namespace nc::graphics
