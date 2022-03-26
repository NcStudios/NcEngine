#include "Environment.h"
#include "assets/AssetService.h"
#include "graphics/Base.h"
#include "ShaderResourceService.h"

namespace nc
{
    Environment::Environment()
    : m_environmentData{},
      m_useSkybox{false}
    {
        m_environmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
        m_environmentData.skyboxTextureIndex = -1;
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

    void Environment::SetCameraPosition(const Vector3& cameraPosition)
    {
        if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
        {
            LoadMeshAsset(SkyboxMeshPath);
        }

        m_environmentData.cameraWorldPosition = cameraPosition;
    }

    bool Environment::UseSkybox()
    {
        return m_useSkybox;
    }

    auto Environment::Get() const -> const EnvironmentData& 
    {
        return m_environmentData;
    }

    void Environment::Clear()
    {
        m_useSkybox = false;
        m_environmentData.skyboxTextureIndex = -1;
    }
}