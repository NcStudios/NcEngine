#include "ecs/component/MeshRenderer.h"
#include "ecs/Registry.h"
#include "assets/AssetService.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, Material material, TechniqueType techniqueType)
    : ComponentBase{entity},
      #ifdef NC_EDITOR_ENABLED
      m_material{std::move(material)},
      #endif
      m_mesh{AssetService<MeshView>::Get()->Acquire(meshUid)},
      m_textureIndices{},
      m_techniqueType{techniqueType}
    {
        #ifdef NC_EDITOR_ENABLED
        m_textureIndices.baseColor = AssetService<TextureView>::Get()->Acquire(m_material.baseColor); // Todo: Make this more generic for materials;
        m_textureIndices.normal = AssetService<TextureView>::Get()->Acquire(m_material.normal);
        m_textureIndices.roughness = AssetService<TextureView>::Get()->Acquire(m_material.roughness);
        #else
        m_textureIndices.baseColor = AssetService<TextureView>::Get()->Acquire(material.baseColor);
        m_textureIndices.normal = AssetService<TextureView>::Get()->Acquire(material.normal);
        m_textureIndices.roughness = AssetService<TextureView>::Get()->Acquire(material.roughness);
        #endif
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer)
    {
        ImGui::Text("Mesh Renderer");
        meshRenderer->GetMaterial().EditorGuiElement();
    }

    void Material::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:");
        ImGui::Text(baseColor.c_str());
        ImGui::Text("Normal:");
        ImGui::Text(normal.c_str());
        ImGui::Text("Roughness:");
        ImGui::Text(roughness.c_str());
    }
    #endif
}