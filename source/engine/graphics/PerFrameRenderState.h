#pragma once

#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/LightSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/WidgetSystem.h"

#include <span>

namespace nc::graphics
{
struct PerFrameRenderState
{
    CameraState cameraState;
    EnvironmentState environmentState;
    LightState lightState;
    ObjectState objectState;
    WidgetState widgetState;
    ParticleState particleState;
};

struct PerFrameRenderStateData
{
    auto operator<=>(const PerFrameRenderStateData&) const = default;

    bool useSkybox;
    uint32_t omniDirLightsCount;
    uint32_t uniDirLightsCount;
    uint32_t meshRenderersCount;
    uint32_t toonRenderersCount;
    uint32_t widgetsCount;
    uint32_t particlesCount;
};
} // namespace nc::graphics
