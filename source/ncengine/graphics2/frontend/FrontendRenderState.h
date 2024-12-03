#pragma once

#include "subsystem/CameraRenderState.h"
#include "subsystem/LightRenderState.h"
#include "subsystem/MeshRendererRenderState.h"
#include "subsystem/PostProcessState.h"
#include "subsystem/WireframeRendererState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRendererRenderState meshRendererState;
    BufferUpdateInfo<MaterialData> materialRenderState;
    LightRenderState lightRenderState;
    PostProcessState postProcessState;
    WireframeRendererRenderState wireframeRenderState;
};
} // namespace nc::graphics
