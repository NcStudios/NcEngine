#include "component/vulkan/MeshRenderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType)
    : ComponentBase{entity},
      m_mesh{nc::graphics::vulkan::ResourceManager::GetMeshAccessor(meshUid)},
      m_material{std::move(material)},
      m_techniqueType{techniqueType},
      m_objectData{}
    {
        m_objectData.baseColorIndex = nc::graphics::vulkan::ResourceManager::GetTextureAccessor(m_material.baseColor); // Todo: Make this more generic for materials;
        m_objectData.normalIndex = nc::graphics::vulkan::ResourceManager::GetTextureAccessor(m_material.normal); // Todo: Make this more generic for materials;
        m_objectData.roughnessIndex = nc::graphics::vulkan::ResourceManager::GetTextureAccessor(m_material.roughness); // Todo: Make this more generic for materials;
        m_objectData.isInitialized = true;
    }

    const nc::graphics::vulkan::Mesh& MeshRenderer::GetMesh() const
    {
        return m_mesh;
    }

    nc::graphics::vulkan::TechniqueType MeshRenderer::GetTechniqueType() const
    {
        return m_techniqueType;
    }

    nc::graphics::vulkan::Material& MeshRenderer::GetMaterial()
    {
        return m_material;
    }

    void MeshRenderer::Update(const  DirectX::FXMMATRIX& viewMatrix, const  DirectX::FXMMATRIX& projectionMatrix)
    {
        m_objectData.model = ActiveRegistry()->Get<Transform>(GetParentEntity())->GetTransformationMatrix();
        m_objectData.normal = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_objectData.model));
        m_objectData.viewProjection = viewMatrix * projectionMatrix;
    }

    ObjectData MeshRenderer::GetObjectData() const
    {
        return m_objectData;
    }
}

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::MeshRenderer>(vulkan::MeshRenderer* meshRenderer)
    {
        ImGui::Text("Mesh Renderer");
        meshRenderer->GetMaterial().EditorGuiElement();
    }
    #endif
}