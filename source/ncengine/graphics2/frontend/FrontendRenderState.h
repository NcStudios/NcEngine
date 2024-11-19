#pragma once

#include "subsystem/CameraRenderState.h"
#include "subsystem/LightRenderState.h"
#include "subsystem/MeshRendererRenderState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRendererRenderState meshRendererState;
    BufferUpdateInfo<MaterialData> materialRenderState;
    LightRenderState lightRenderState;
};
} // namespace nc::graphics
