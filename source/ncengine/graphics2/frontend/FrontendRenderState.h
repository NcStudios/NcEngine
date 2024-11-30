#pragma once

#include "subsystem/CameraRenderState.h"
#include "subsystem/LightRenderState.h"
#include "subsystem/MeshRendererRenderState.h"
#include "subsystem/PostProcessState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRendererRenderState meshRendererState;
    BufferUpdateInfo<MaterialData> materialRenderState;
    LightRenderState lightRenderState;
    PostProcessState postProcessState;
};
} // namespace nc::graphics
