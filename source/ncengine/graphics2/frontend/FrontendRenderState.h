#pragma once

#include "subsystem/CameraRenderState.h"
#include "subsystem/MeshRendererRenderState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRendererRenderState meshRendererState;
    BufferUpdateInfo<MaterialData> materialRenderState;
};
} // namespace nc::graphics
