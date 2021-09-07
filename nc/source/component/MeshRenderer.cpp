#include "component/MeshRenderer.h"
#include "component/Transform.h"
#include "graphics/resources/ResourceManager.h"
#include "Ecs.h"

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
      m_mesh{nc::graphics::ResourceManager::GetMeshAccessor(meshUid)},
      m_textureIndices{},
      m_techniqueType{techniqueType}
    {
        #ifdef NC_EDITOR_ENABLED
        m_textureIndices.baseColor = nc::graphics::ResourceManager::GetTextureAccessor(m_material.baseColor); // Todo: Make this more generic for materials;
        m_textureIndices.normal = nc::graphics::ResourceManager::GetTextureAccessor(m_material.normal);       // Todo: Make this more generic for materials;
        m_textureIndices.roughness = nc::graphics::ResourceManager::GetTextureAccessor(m_material.roughness); // Todo: Make this more generic for materials;
        #else
        m_textureIndices.baseColor = nc::graphics::ResourceManager::GetTextureAccessor(material.baseColor);
        m_textureIndices.normal = nc::graphics::ResourceManager::GetTextureAccessor(material.normal);
        m_textureIndices.roughness = nc::graphics::ResourceManager::GetTextureAccessor(material.roughness);
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