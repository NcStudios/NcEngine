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
      m_textureIndices{}
    {
        #ifdef NC_EDITOR_ENABLED
        m_textureIndices.baseColor = AssetService<TextureView>::Get()->Acquire(m_material.baseColor);
        m_textureIndices.overlay = AssetService<TextureView>::Get()->Acquire(m_material.overlay);
        m_textureIndices.lightShading = AssetService<TextureView>::Get()->Acquire(m_material.lightShading);
        m_textureIndices.heavyShading = AssetService<TextureView>::Get()->Acquire(m_material.heavyShading);
        #else
        m_textureIndices.baseColor = AssetService<TextureView>::Get()->Acquire(material.baseColor);
        m_textureIndices.overlay = AssetService<TextureView>::Get()->Acquire(material.overlay);
        m_textureIndices.lightShading = AssetService<TextureView>::Get()->Acquire(material.lightShading);
        m_textureIndices.heavyShading = AssetService<TextureView>::Get()->Acquire(material.heavyShading);
        #endif
    }

    #ifdef NC_EDITOR_ENABLED
    void ToonMaterial::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:");
        ImGui::Text(baseColor.c_str());
        ImGui::Text("Overlay:");
        ImGui::Text(overlay.c_str());
        ImGui::Text("Light Shading:");
        ImGui::Text(lightShading.c_str());
        ImGui::Text("Heavy Shading:");
        ImGui::Text(heavyShading.c_str());
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