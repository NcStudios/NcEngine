#include "EnvironmentSystem.h"
#include "assets/AssetService.h"
#include "graphics/shader_resource/ShaderTypes.h"
#include "CameraSystem.h"

namespace nc::graphics
{
EnvironmentSystem::EnvironmentSystem(ShaderResourceBus* shaderResourceBus)
    : m_environmentData{},
      m_environmentDataBuffer{shaderResourceBus->CreateUniformBuffer(sizeof(EnvironmentData), ShaderStage::Fragment | ShaderStage::Vertex, 5, 0)},
      m_useSkybox{false}
{
}

void EnvironmentSystem::SetSkybox(const std::string& path)
{
    m_useSkybox = true;
    auto skyboxView = AssetService<CubeMapView>::Get()->Acquire(path);
    m_environmentData.skyboxTextureIndex = skyboxView.index;
}

auto EnvironmentSystem::Execute(const CameraState& cameraState, uint32_t currentFrameIndex) -> EnvironmentState
{
    m_environmentData.cameraWorldPosition = Vector4(cameraState.position, 0.0f);
    m_environmentDataBuffer.Update(static_cast<void*>(&m_environmentData), currentFrameIndex);
    return EnvironmentState{m_useSkybox};
}

void EnvironmentSystem::Clear()
{
    m_useSkybox = false;
    m_environmentData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
}
} // namespace nc::graphics
