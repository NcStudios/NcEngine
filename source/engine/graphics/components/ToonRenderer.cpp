#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/ToonRenderer.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::graphics
{
    ToonRenderer::ToonRenderer(Entity entity, std::string meshUid, ToonMaterial material)
    : ComponentBase{entity},
      #ifdef NC_EDITOR_ENABLED
      m_material{std::move(material)},
      m_meshPath{meshUid},
      #endif
      m_mesh{AssetService<MeshView>::Get()->Acquire(meshUid)},
      m_materialView{}
    {
        #ifdef NC_EDITOR_ENABLED
        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(m_material.baseColor);
        m_materialView.overlay = AssetService<TextureView>::Get()->Acquire(m_material.overlay);
        m_materialView.hatching = AssetService<TextureView>::Get()->Acquire(m_material.hatching);
        m_materialView.hatchingTiling = m_material.hatchingTiling;
        #else
        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(material.baseColor);
        m_materialView.overlay = AssetService<TextureView>::Get()->Acquire(material.overlay);
        m_materialView.hatching = AssetService<TextureView>::Get()->Acquire(material.hatching);
        m_materialView.hatchingTiling = material.hatchingTiling;
        #endif
    }

    #ifdef NC_EDITOR_ENABLED
    void ToonMaterial::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:");
        ImGui::Text("%s", baseColor.c_str());
        ImGui::Text("Overlay:");
        ImGui::Text("%s", overlay.c_str());
        ImGui::Text("Hatching Texture:");
        ImGui::Text("%s", hatching.c_str());
        ImGui::Text("Hatching Tiling:");
        ImGui::Text("%u", hatchingTiling);
    }
    #endif
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::ToonRenderer>(graphics::ToonRenderer* toonRenderer)
{
    ImGui::Text("Toon Renderer");
    toonRenderer->GetMaterial().EditorGuiElement();
}
#endif
} // namespace nc
