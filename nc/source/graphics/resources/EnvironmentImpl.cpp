#include "graphics/Environment.h"
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

    void Environment::Set(const CubeMapFaces& skybox)
    {
        m_useSkybox = true;

        if (!AssetService<MeshView>::Get()->IsLoaded(SkyboxMeshPath))
        {
            LoadMeshAsset(SkyboxMeshPath);
        }

        auto skyboxView = AssetService<CubeMapView, CubeMapFaces>::Get()->Acquire(skybox);
        m_environmentData.skyboxTextureIndex = skyboxView.index;
    }

    void Environment::Set(const Vector3& cameraPosition)
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

    const EnvironmentData& Environment::Get() const
    {
        return m_environmentData;
    }

    void Environment::Clear()
    {
        m_useSkybox = false;
        m_environmentData.skyboxTextureIndex = -1;
    }
}