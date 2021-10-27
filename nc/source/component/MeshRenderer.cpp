#include "Ecs.h"
#include "assets/AssetService.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, nc::graphics::Material material, nc::graphics::TechniqueType techniqueType)
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
        m_textureIndices.metallic = AssetService<TextureView>::Get()->Acquire(m_material.metallic);
        #else
        m_textureIndices.baseColor = AssetService<TextureView>::Get()->Acquire(material.baseColor);
        m_textureIndices.normal = AssetService<TextureView>::Get()->Acquire(material.normal);
        m_textureIndices.roughness = AssetService<TextureView>::Get()->Acquire(material.roughness);
        m_textureIndices.metallic = AssetService<TextureView>::Get()->Acquire(material.metallic);
        #endif
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer)
    {
        ImGui::Text("Mesh Renderer");
        meshRenderer->GetMaterial().EditorGuiElement();
    }
    #endif
}