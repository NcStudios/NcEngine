#pragma once

#include "subsystem/CameraSubsystem.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc::graphics
{
// todo: move AssetDispatch here...

struct FrontendRenderState;

struct GraphicsFrontend
{
    CameraSubsystem camera;

    auto BuildRenderState(ecs::Ecs world) -> FrontendRenderState;

    void Clear() noexcept
    {
        camera.Clear();
    }
};
} // namespace nc::graphics
