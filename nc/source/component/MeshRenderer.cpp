#include "Ecs.h"
#include "graphics/resources/ResourceManager.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, graphics::Material material, graphics::TechniqueType techniqueType)
    : ComponentBase{entity},
      #ifdef NC_EDITOR_ENABLED
      m_material{std::move(material)},
      m_meshPath{meshUid},
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

    void MeshRenderer::SetMesh(std::string meshUid)
    {
        #ifdef NC_EDITOR_ENABLED
        m_meshPath = meshUid;
        #endif

        m_mesh = graphics::ResourceManager::GetMeshAccessor(meshUid);
    }

    void MeshRenderer::SetBaseColor(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.baseColor = texturePath;
        #endif

        m_textureIndices.baseColor = graphics::ResourceManager::GetTextureAccessor(texturePath);
    }

    void MeshRenderer::SetNormal(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.normal = texturePath;
        #endif

        m_textureIndices.normal = graphics::ResourceManager::GetTextureAccessor(texturePath);
    }

    void MeshRenderer::SetRoughness(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.roughness = texturePath;
        #endif

        m_textureIndices.roughness = graphics::ResourceManager::GetTextureAccessor(texturePath);
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer)
    {
        ImGui::Text("Mesh Renderer");
        meshRenderer->GetMaterial().EditorGuiElement();
    }
    #endif
}