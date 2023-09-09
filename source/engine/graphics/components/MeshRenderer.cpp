#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/MeshRenderer.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
auto MakeMaterialView(const nc::graphics::PbrMaterial& material) -> nc::graphics::PbrMaterialView
{
    const auto textureService = nc::AssetService<nc::TextureView>::Get();
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
          m_mesh{AssetService<MeshView>::Get()->Acquire(meshUid)},
          m_materialView{::MakeMaterialView(material)},
          m_techniqueType{techniqueType},
          m_coldData{std::make_unique<MeshRendererColdData>(std::move(meshUid), std::move(material))}
    {
    }

    void MeshRenderer::SetMesh(std::string meshUid)
    {
        m_mesh = AssetService<MeshView>::Get()->Acquire(meshUid);
        m_coldData->meshPath = std::move(meshUid);
    }

    void MeshRenderer::SetMaterial(PbrMaterial material)
    {
        m_materialView = ::MakeMaterialView(material);
        m_coldData->material = std::move(material);
    }

    void MeshRenderer::SetBaseColor(std::string texturePath)
    {
        m_materialView.baseColor = AssetService<TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.baseColor = std::move(texturePath);
    }

    void MeshRenderer::SetNormal(std::string texturePath)
    {
        m_materialView.normal = AssetService<TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.normal = std::move(texturePath);
    }

    void MeshRenderer::SetRoughness(std::string texturePath)
    {
        m_materialView.roughness = AssetService<TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.roughness = std::move(texturePath);
    }

    void MeshRenderer::SetMetallic(std::string texturePath)
    {
        m_materialView.metallic = AssetService<TextureView>::Get()->Acquire(texturePath);
        m_coldData->material.metallic = std::move(texturePath);
    }

    #ifdef NC_EDITOR_ENABLED
    void PbrMaterial::EditorGuiElement() const
    {
        // TODO: try sameline Base Color:\n%s ...

        // ImGui::SameLine();


        ImGui::Text("Material");
        ImGui::Spacing();
        ImGui::Text("Base Color:\n\t%s", baseColor.c_str());
        ImGui::Text("Normal:\n\t%s", normal.c_str());
        ImGui::Text("Roughness:\n\t%s", roughness.c_str());
        ImGui::Text("Metallic:\n\t%s", metallic.c_str());
    }
    #endif
} // namespace nc::graphics

namespace nc
{


// TODO: add mesh uid???

#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::MeshRenderer>(graphics::MeshRenderer* meshRenderer)
{
    ImGui::Text("Mesh Renderer");

    ImGui::Text("Mesh");
    ImGui::Spacing();
    ImGui::Text("\t%s", meshRenderer->GetMeshPath().c_str());

    meshRenderer->GetMaterial().EditorGuiElement();
}
#endif
} // namespace nc
