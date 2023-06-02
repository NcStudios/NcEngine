#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/MeshRenderer.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::graphics
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, PbrMaterial material, TechniqueType techniqueType)
    : ComponentBase{entity},
      #ifdef NC_EDITOR_ENABLED
      m_material{std::move(material)},
      m_meshPath{meshUid},
      #endif
      m_mesh{AssetService<MeshView>::Get()->Acquire(meshUid)},
      m_materialView{},
      m_techniqueType{techniqueType}
    {
        #ifdef NC_EDITOR_ENABLED
        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(m_material.baseColor); // Todo: Make this more generic for materials;
        m_materialView.normal = AssetService<TextureView>::Get()->Acquire(m_material.normal);
        m_materialView.roughness = AssetService<TextureView>::Get()->Acquire(m_material.roughness);
        m_materialView.metallic = AssetService<TextureView>::Get()->Acquire(m_material.metallic);
        #else
        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(material.baseColor);
        m_materialView.normal = AssetService<TextureView>::Get()->Acquire(material.normal);
        m_materialView.roughness = AssetService<TextureView>::Get()->Acquire(material.roughness);
        m_materialView.metallic = AssetService<TextureView>::Get()->Acquire(material.metallic);
        #endif
    }

    void MeshRenderer::SetMesh(std::string meshUid)
    {
        #ifdef NC_EDITOR_ENABLED
        m_meshPath = meshUid;
        #endif

        m_mesh = AssetService<MeshView>::Get()->Acquire(meshUid);
    }

    void MeshRenderer::SetBaseColor(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.baseColor = texturePath;
        #endif

        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(texturePath);
    }

    void MeshRenderer::SetNormal(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.normal = texturePath;
        #endif

        m_materialView.normal = AssetService<TextureView>::Get()->Acquire(texturePath);
    }

    void MeshRenderer::SetRoughness(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.roughness = texturePath;
        #endif

        m_materialView.roughness = AssetService<TextureView>::Get()->Acquire(texturePath);
    }

    void MeshRenderer::SetMetallic(const std::string& texturePath)
    {
        #ifdef NC_EDITOR_ENABLED
        m_material.metallic = texturePath;
        #endif

        m_materialView.metallic = AssetService<TextureView>::Get()->Acquire(texturePath);
    }

    #ifdef NC_EDITOR_ENABLED
    void PbrMaterial::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:");
        ImGui::Text("%s", baseColor.c_str());
        ImGui::Text("Normal:");
        ImGui::Text("%s", normal.c_str());
        ImGui::Text("Roughness:");
        ImGui::Text("%s", roughness.c_str());
    }
    #endif
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::MeshRenderer>(graphics::MeshRenderer* meshRenderer)
{
    ImGui::Text("Mesh Renderer");
    meshRenderer->GetMaterial().EditorGuiElement();
}
#endif
} // namespace nc
