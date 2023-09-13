#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/ToonRenderer.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
auto MakeMaterialView(const nc::graphics::ToonMaterial& material) -> nc::graphics::ToonMaterialView
{
    const auto textureService = nc::AssetService<nc::TextureView>::Get();
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
      m_meshView{AssetService<MeshView>::Get()->Acquire(meshUid)},
      m_materialView{::MakeMaterialView(material)},
      m_coldData{std::make_unique<ToonRendererColdData>(std::move(meshUid), std::move(material))}
{
}

void ToonRenderer::SetMesh(std::string meshUid)
{
    m_meshView = AssetService<MeshView>::Get()->Acquire(meshUid);
    m_coldData->meshPath = std::move(meshUid);
}

void ToonRenderer::SetMaterial(ToonMaterial material)
{
    m_materialView = ::MakeMaterialView(material);
    m_coldData->material = std::move(material);
}

void ToonRenderer::SetBaseColor(std::string texturePath)
{
    m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.baseColor = std::move(texturePath);
}

void ToonRenderer::SetOverlay(std::string texturePath)
{
    m_materialView.overlay = AssetService<TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.overlay = std::move(texturePath);
}

void ToonRenderer::SetHatching(std::string texturePath)
{
    m_materialView.hatching = AssetService<TextureView>::Get()->Acquire(texturePath);
    m_coldData->material.hatching = std::move(texturePath);
}

void ToonRenderer::SetHatchingTiling(uint32_t tiling)
{
    m_materialView.hatchingTiling = tiling;
    m_coldData->material.hatchingTiling = tiling;
}
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::ToonRenderer>(graphics::ToonRenderer* toonRenderer)
{
    ImGui::Text("Toon Renderer");
    ImGui::Text("Mesh: %s", toonRenderer->GetMeshPath().c_str());
    const auto& material = toonRenderer->GetMaterial();
    ImGui::Text("Material:");
    ImGui::Text("  Base Color: %s", material.baseColor.c_str());
    ImGui::Text("  Overlay: %s", material.overlay.c_str());
    ImGui::Text("  Hatching Texture: %s", material.hatching.c_str());
    ImGui::Text("  Hatching Tiling: %u", material.hatchingTiling);
}
#endif
} // namespace nc
