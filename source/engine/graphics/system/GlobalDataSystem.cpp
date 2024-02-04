#include "GlobalDataSystem.h"
#include "assets/AssetService.h"
#include "CameraSystem.h"

namespace nc::graphics
{
GlobalDataSystem::GlobalDataSystem(Signal<const GlobalData&>&& backendChannel)
    : m_backendChannel{std::move(backendChannel)},
      m_globalData{},
      m_useSkybox{false}
{
    m_globalData.cameraWorldPos = Vector3{-0.0f, 4.0f, -6.4f};
    m_globalData.skyboxTextureIndex = 0u;
}

void GlobalDataSystem::SetSkybox(const std::string& path)
{
    m_useSkybox = true;
    auto skyboxView = AssetService<CubeMapView>::Get()->Acquire(path);
    m_globalData.skyboxTextureIndex = skyboxView.index;
}

auto GlobalDataSystem::Execute(const CameraState& cameraState) -> GlobalDataState
{
    m_globalData.cameraViewProjMat = cameraState.view * cameraState.projection;
    m_globalData.cameraWorldPos = cameraState.position;
    m_backendChannel.Emit(m_globalData);
    return GlobalDataState{m_useSkybox};
}

void GlobalDataSystem::Clear()
{
    m_useSkybox = false;
    m_globalData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
}
} // namespace nc::graphics
