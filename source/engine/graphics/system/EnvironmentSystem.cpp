#include "EnvironmentSystem.h"
#include "assets/AssetService.h"
#include "CameraSystem.h"

namespace nc::graphics
{
EnvironmentSystem::EnvironmentSystem(Signal<const EnvironmentData&>&& backendChannel)
    : m_backendChannel{std::move(backendChannel)},
      m_environmentData{},
      m_useSkybox{false}
{
    m_environmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
    m_environmentData.skyboxTextureIndex = 0u;
}

void EnvironmentSystem::SetSkybox(const std::string& path)
{
    m_useSkybox = true;
    auto skyboxView = AssetService<CubeMapView>::Get()->Acquire(path);
    m_environmentData.skyboxTextureIndex = skyboxView.index;
}

auto EnvironmentSystem::Execute(const CameraState& cameraState) -> EnvironmentState
{
    m_environmentData.cameraWorldPosition = cameraState.position;
    m_backendChannel.Emit(m_environmentData);
    return EnvironmentState{m_useSkybox};
}

void EnvironmentSystem::Clear()
{
    m_useSkybox = false;
    m_environmentData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
}
} // namespace nc::graphics
