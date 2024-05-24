#pragma once

#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/PointLightSystem.h"
#include "system/SpotLightSystem.h"
#include "system/WidgetSystem.h"

#include <span>

namespace nc::graphics
{
struct PerFrameRenderState
{
    CameraState cameraState;
    EnvironmentState environmentState;
    ObjectState objectState;
    PointLightState pointLightState;
    SpotLightState spotLightState;
    WidgetState widgetState;
    ParticleState particleState;
};
} // namespace nc::graphics
