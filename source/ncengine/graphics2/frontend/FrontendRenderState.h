#pragma once

#include "subsystem/CameraRenderState.h"
#include "subsystem/LightRenderState.h"
#include "subsystem/MeshRenderState.h"
#include "subsystem/PostProcessState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRenderState meshRenderState;
    BufferUpdateInfo<MaterialData> materialRenderState;
    LightRenderState lightRenderState;
    PostProcessState postProcessState;
};
} // namespace nc::graphics
