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
class Camera;
class Environment;

struct PerFrameRenderState
{
    PerFrameRenderState(Registry* registry, Camera* camera, bool isPointLightSystemDirty, Environment* environment, std::span<const nc::particle::EmitterState> particleEmitters);

    DirectX::XMMATRIX camViewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    Vector3 cameraPosition;
    std::vector<ObjectData> objectData;
    std::vector<MeshView> meshes;
    std::vector<PointLightInfo> pointLightInfos;
    #ifdef NC_EDITOR_ENABLED
    std::optional<nc::graphics::DebugWidget> colliderDebugWidget;
    #endif
    std::vector<DirectX::XMMATRIX> pointLightVPs;
    bool isPointLightBindRequired;
    Environment* environment;
    bool useSkybox;
    std::span<const nc::particle::EmitterState> emitterStates;
};

void MapPerFrameRenderState(const PerFrameRenderState& state);
} // namespace graphics
} // namespace nc
