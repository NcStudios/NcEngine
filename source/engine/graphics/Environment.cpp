#include "Environment.h"
#include "assets/AssetService.h"

namespace nc::graphics
{
Environment::Environment(Signal<const EnvironmentData&>&& backendChannel)
    : m_backendChannel{std::move(backendChannel)},
      m_environmentData{},
      m_useSkybox{false}
{
    m_environmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
    m_environmentData.skyboxTextureIndex = 0u;
}

void Environment::SetSkybox(const std::string& path)
{
    m_useSkybox = true;

    if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
    {
        LoadMeshAsset(SkyboxMeshPath);
    }

    auto skyboxView = AssetService<CubeMapView>::Get()->Acquire(path);
    m_environmentData.skyboxTextureIndex = skyboxView.index;
}

// auto Environment::Get() const -> const EnvironmentData&
// {
//     return m_environmentData;
// }

auto Environment::Execute(const Vector3& cameraPosition) -> EnvironmentFrontendState
{
    if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
    {
        LoadMeshAsset(SkyboxMeshPath);
    }

    m_environmentData.cameraWorldPosition = cameraPosition;
    m_backendChannel.Emit(m_environmentData);
    return EnvironmentFrontendState{m_useSkybox};
}

void Environment::Clear()
{
    m_useSkybox = false;
    m_environmentData.skyboxTextureIndex = -1;
}
} // namespace nc::graphics
