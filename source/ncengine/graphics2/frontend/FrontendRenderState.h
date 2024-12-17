#pragma once

#include "subsystem/animation/SkeletalAnimationRenderState.h"
#include "subsystem/CameraRenderState.h"
#include "subsystem/LightRenderState.h"
#include "subsystem/MeshRenderState.h"
#include "subsystem/PostProcessState.h"
#include "subsystem/WireframeRendererState.h"

namespace nc::graphics
{
struct FrontendRenderState
{
    CameraRenderState cameraState;
    MeshRenderState meshRenderState;
    SkeletalAnimationRenderState animationRenderState;
    BufferUpdateInfo<MaterialData> materialRenderState;
    LightRenderState lightRenderState;
    PostProcessState postProcessState;
    WireframeRendererRenderState wireframeRenderState;
};
} // namespace nc::graphics
