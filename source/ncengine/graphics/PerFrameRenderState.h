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

    bool useSkybox = false;
    uint32_t omniDirLightsCount = 0u;
    uint32_t uniDirLightsCount = 0u;
    uint32_t meshRenderersCount = 0u;
    uint32_t toonRenderersCount = 0u;
    uint32_t widgetsCount = 0u;
    uint32_t particlesCount = 0u;
};

struct PerFrameInstanceData
{
    uint32_t shadowCasterIndex = 0u;
    uint32_t passIndex = 0u;
    bool isOmniDirectional = false;
};
} // namespace nc::graphics
