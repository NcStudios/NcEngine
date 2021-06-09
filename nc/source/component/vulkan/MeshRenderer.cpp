#include "component/vulkan/MeshRenderer.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    MeshRenderer::MeshRenderer(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType)
    : Component{parentHandle},
      m_meshUid{std::move(meshUid)},
      m_material{std::move(material)},
      m_techniqueType{techniqueType}
    {
    }

    Transform* MeshRenderer::GetTransform()
    {
        return GetComponent<Transform>(GetParentHandle());
    }

    const std::string& MeshRenderer::GetMeshUid() const
    {
        return m_meshUid;
    }

    const nc::graphics::vulkan::Material& MeshRenderer::GetMaterial() const
    {
        return m_material;
    }

    nc::graphics::vulkan::TechniqueType MeshRenderer::GetTechniqueType() const
    {
        return m_techniqueType;
    }
    
    #ifdef NC_EDITOR_ENABLED
    void MeshRenderer::EditorGuiElement()
    {
        ImGui::Text("Mesh Renderer");
        m_material.EditorGuiElement();
    }
    #endif
}