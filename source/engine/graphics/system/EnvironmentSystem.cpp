#include "EnvironmentSystem.h"
#include "assets/AssetService.h"
#include "graphics/shader_resource/ShaderTypes.h"
#include "CameraSystem.h"

namespace nc::graphics
{
EnvironmentSystem::EnvironmentSystem(ShaderResourceBus* shaderResourceBus)
    : m_environmentData{},
      m_environmentDataBuffer{shaderResourceBus->CreateUniformBuffer("EnvironmentData", (char*)&m_environmentData, 5, ShaderStage::Fragment | ShaderStage::Vertex)},
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
    m_environmentDataBuffer.Update((char*)&m_environmentData, currentFrameIndex);
    return EnvironmentState{m_useSkybox};
}

void EnvironmentSystem::Clear()
{
    m_useSkybox = false;
    m_environmentData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
    m_environmentDataBuffer.Clear();
}
} // namespace nc::graphics
