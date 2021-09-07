#include "Ecs.h"
#include "graphics/resources/ResourceManager.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, nc::graphics::Material material, nc::graphics::TechniqueType techniqueType)
    : ComponentBase{entity},
      m_mesh{nc::graphics::ResourceManager::GetMeshAccessor(meshUid)},
      m_material{std::move(material)},
      m_techniqueType{techniqueType},
      m_objectData{}
    {
        m_objectData.baseColorIndex = nc::graphics::ResourceManager::GetTextureAccessor(m_material.baseColor); // Todo: Make this more generic for materials;
        m_objectData.normalIndex = nc::graphics::ResourceManager::GetTextureAccessor(m_material.normal); // Todo: Make this more generic for materials;
        m_objectData.roughnessIndex = nc::graphics::ResourceManager::GetTextureAccessor(m_material.roughness); // Todo: Make this more generic for materials;
        m_objectData.isInitialized = true;
    }

    const nc::graphics::Mesh& MeshRenderer::GetMesh() const
    {
        return m_mesh;
    }

    nc::graphics::TechniqueType MeshRenderer::GetTechniqueType() const
    {
        return m_techniqueType;
    }

    nc::graphics::Material& MeshRenderer::GetMaterial()
    {
        return m_material;
    }

    void MeshRenderer::Update(Transform* transform, const DirectX::FXMMATRIX& viewMatrix, const  DirectX::FXMMATRIX& projectionMatrix)
    {
        m_objectData.model = transform->GetTransformationMatrix();
        m_objectData.modelView = m_objectData.model * viewMatrix;
        m_objectData.viewProjection = viewMatrix * projectionMatrix;
    }

    const ObjectData& MeshRenderer::GetObjectData() const
    {
        return m_objectData;
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer)
    {
        ImGui::Text("Mesh Renderer");
        meshRenderer->GetMaterial().EditorGuiElement();
    }
    #endif
}