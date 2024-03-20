#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/MeshRenderer.h"

namespace
{
auto MakeMaterialView(const nc::graphics::PbrMaterial& material) -> nc::graphics::PbrMaterialView
{
    const auto textureService = nc::asset::AssetService<nc::asset::TextureView>::Get();
    return nc::graphics::PbrMaterialView
    {
        textureService->Acquire(material.baseColor),
        textureService->Acquire(material.normal),
        textureService->Acquire(material.roughness),
        textureService->Acquire(material.metallic)
    };
}
} // anonymous namespace

namespace nc::graphics
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, PbrMaterial material, TechniqueType techniqueType)
        : ComponentBase{entity},
          m_meshView{asset::AssetService<asset::MeshView>::Get()->Acquire(meshUid)},
          m_materialView{::MakeMaterialView(material)},
          m_techniqueType{techniqueType},
          m_coldData{std::make_unique<MeshRendererColdData>(std::move(meshUid), std::move(material))}
    {
    }

    void MeshRenderer::SetMesh(std::string meshUid)
    {
        m_meshView = asset::AssetService<asset::MeshView>::Get()->Acquire(meshUid);
        m_coldData->meshPath = std::move(meshUid);
    }

    void MeshRenderer::SetMaterial(PbrMaterial material)
    {
        m_materialView = ::MakeMaterialView(material);
        m_coldData->material = std::move(material);
    }

    void MeshRenderer::SetBaseColor(std::string texturePath)
    {
        m_materialView.baseColor = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.baseColor = std::move(texturePath);
    }

    void MeshRenderer::SetNormal(std::string texturePath)
    {
        m_materialView.normal = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.normal = std::move(texturePath);
    }

    void MeshRenderer::SetRoughness(std::string texturePath)
    {
        m_materialView.roughness = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.roughness = std::move(texturePath);
    }

    void MeshRenderer::SetMetallic(std::string texturePath)
    {
        m_materialView.metallic = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.metallic = std::move(texturePath);
    }
} // namespace nc::graphics
