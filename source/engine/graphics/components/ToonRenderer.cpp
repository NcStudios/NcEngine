#include "asset/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/ToonRenderer.h"

namespace
{
auto MakeMaterialView(const nc::graphics::ToonMaterial& material) -> nc::graphics::ToonMaterialView
{
    const auto textureService = nc::asset::AssetService<nc::asset::TextureView>::Get();
    return nc::graphics::ToonMaterialView
    {
        textureService->Acquire(material.baseColor),
        textureService->Acquire(material.overlay),
        textureService->Acquire(material.hatching),
        material.hatchingTiling
    };
}
} // anonymous namespace

namespace nc::graphics
{
ToonRenderer::ToonRenderer(Entity entity, std::string meshUid, ToonMaterial material)
    : ComponentBase{entity},
      m_meshView{asset::AssetService<asset::MeshView>::Get()->Acquire(meshUid)},
      m_materialView{::MakeMaterialView(material)},
      m_coldData{std::make_unique<ToonRendererColdData>(std::move(meshUid), std::move(material))}
{
}

void ToonRenderer::SetMesh(std::string meshUid)
{
    m_meshView = asset::AssetService<asset::MeshView>::Get()->Acquire(meshUid);
    m_coldData->meshPath = std::move(meshUid);
}

void ToonRenderer::SetMaterial(ToonMaterial material)
{
    m_materialView = ::MakeMaterialView(material);
    m_coldData->material = std::move(material);
}

void ToonRenderer::SetBaseColor(std::string texturePath)
{
    m_materialView.baseColor = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.baseColor = std::move(texturePath);
}

void ToonRenderer::SetOverlay(std::string texturePath)
{
    m_materialView.overlay = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.overlay = std::move(texturePath);
}

void ToonRenderer::SetHatching(std::string texturePath)
{
    m_materialView.hatching = asset::AssetService<asset::TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.hatching = std::move(texturePath);
}

void ToonRenderer::SetHatchingTiling(uint32_t tiling)
{
    m_materialView.hatchingTiling = tiling;
    m_coldData->material.hatchingTiling = tiling;
}
} // namespace nc::graphics
