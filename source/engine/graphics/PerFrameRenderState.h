#pragma once

#include "particle/EmitterState.h"
#include "system/CameraSystem.h"
#include "system/GlobalDataSystem.h"
#include "system/ObjectSystem.h"
#include "system/PointLightSystem.h"
#include "system/WidgetSystem.h"

#include <span>

namespace nc::graphics
{
struct PerFrameRenderState
{
    CameraState cameraState;
    GlobalDataState globalDataState;
    ObjectState objectState;
    LightingState lightingState;
    WidgetState widgetState;
    std::span<const nc::particle::EmitterState> emitterStates;
};
} // namespace nc::graphics
