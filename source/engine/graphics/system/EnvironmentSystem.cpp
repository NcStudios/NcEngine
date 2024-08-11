#include "EnvironmentSystem.h"
#include "asset/AssetService.h"
#include "graphics/shader_resource/ShaderTypes.h"
#include "CameraSystem.h"

#include <span>

namespace nc::graphics
{
EnvironmentSystem::EnvironmentSystem(ShaderResourceBus* shaderResourceBus)
    : m_environmentData{},
      m_environmentDataBuffer{shaderResourceBus->CreateUniformBuffer(sizeof(EnvironmentData), ShaderStage::Fragment | ShaderStage::Vertex, 5, 0, false)},
      m_useSkybox{false}
{
}

void EnvironmentSystem::SetSkybox(const std::string& path)
{
    m_useSkybox = true;
    auto skyboxView = asset::AssetService<asset::CubeMapView>::Get()->Acquire(path);
    m_environmentData.skyboxTextureIndex = skyboxView.index;
}

auto EnvironmentSystem::Execute(const CameraState& cameraState, uint32_t currentFrameIndex) -> EnvironmentState
{
    m_environmentData.cameraViewProjection = cameraState.view * cameraState.projection;
    m_environmentData.cameraWorldPosition = Vector4(cameraState.position, 0.0f);
    m_environmentDataBuffer.Update(std::span{&m_environmentData, 1}, currentFrameIndex);
    return EnvironmentState{m_useSkybox, m_useShadowTest};
}

void EnvironmentSystem::Clear()
{
    m_useSkybox = false;
    m_environmentData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
}
} // namespace nc::graphics
