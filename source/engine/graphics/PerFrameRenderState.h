#pragma once

#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/PointLightSystem.h"
#include "system/WidgetSystem.h"

#include <span>

namespace nc::graphics
{
struct PerFrameRenderState
{
    CameraState cameraState;
    EnvironmentState environmentState;
    ObjectState objectState;
    LightingState lightingState;
    WidgetState widgetState;
    ParticleState particleState;
};
} // namespace nc::graphics
