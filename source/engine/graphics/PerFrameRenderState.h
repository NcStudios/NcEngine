#pragma once

#include "asset/Assets.h"
#include "ecs/Registry.h"
#include "graphics/PointLight.h"
#include "graphics/shader_resource/ObjectData.h"
#include "particle/EmitterState.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/DebugWidget.h"
#endif

#include <span>

namespace nc
{
class Registry;

#ifdef NC_DEBUG_RENDERING_ENABLED
namespace physics
{
class PhysicsSystemImpl;
}
#endif

namespace graphics
{

struct CameraState;
struct EnvironmentState;
struct LightingState;
struct ObjectState;
struct WidgetSystemState;

struct PerFrameRenderState
{
    PerFrameRenderState(CameraState&& cameraState,
                        EnvironmentState&& environmentState,
                        ObjectState&& objectState,
                        LightingState&& lightingState,
                        WidgetSystemState&& widgetState,
                        std::span<const nc::particle::EmitterState> particleEmitters);

    DirectX::XMMATRIX camViewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    Vector3 cameraPosition;
    std::vector<MeshView> meshes;
    #ifdef NC_EDITOR_ENABLED
    std::optional<nc::graphics::DebugWidget> colliderDebugWidget;
    #endif
    std::span<const DirectX::XMMATRIX> pointLightVPs;
    bool useSkybox;
    std::span<const nc::particle::EmitterState> emitterStates;
    uint32_t skyboxInstanceIndex = -1;
};

} // namespace graphics
} // namespace nc
