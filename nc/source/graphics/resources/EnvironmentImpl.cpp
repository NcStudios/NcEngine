#include "EnvironmentImpl.h"
#include "assets/AssetService.h"
#include "graphics/Base.h"
#include "ShaderResourceService.h"

namespace nc
{
    EnvironmentImpl::EnvironmentImpl()
    : m_environmentData{},
      m_useSkybox{false}
    {
        m_environmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
        m_environmentData.skyboxTextureIndex = -1;
    }

    void EnvironmentImpl::SetSkybox(const CubeMapFaces& skybox)
    {
        m_useSkybox = true;

        if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
        {
            LoadMeshAsset(SkyboxMeshPath);
        }

        auto skyboxView = AssetService<CubeMapView, CubeMapFaces>::Get()->Acquire(skybox);
        m_environmentData.skyboxTextureIndex = skyboxView.index;
    }

    void EnvironmentImpl::SetCameraPosition(const Vector3& cameraPosition)
    {
        if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
        {
            LoadMeshAsset(SkyboxMeshPath);
        }

        m_environmentData.cameraWorldPosition = cameraPosition;
    }

    bool EnvironmentImpl::UseSkybox()
    {
        return m_useSkybox;
    }

    const EnvironmentData& EnvironmentImpl::Get() const
    {
        return m_environmentData;
    }

    void EnvironmentImpl::Clear()
    {
        m_useSkybox = false;
        m_environmentData.skyboxTextureIndex = -1;
    }
}