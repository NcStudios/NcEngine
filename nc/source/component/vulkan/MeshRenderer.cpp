#include "component/vulkan/MeshRenderer.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    MeshRenderer::MeshRenderer(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::PhongMaterial material, Transform* transform)
    : Component{parentHandle},
      m_transform{transform},
      m_meshUid{meshUid},
      m_material(material)
    {
    }

    Transform* MeshRenderer::GetTransform()
    {
        return m_transform;
    }

    const std::string& MeshRenderer::GetMeshUid()
    {
        return m_meshUid;
    }

    const nc::graphics::vulkan::PhongMaterial& MeshRenderer::GetMaterial()
    {
        return m_material;
    }
    
    #ifdef NC_EDITOR_ENABLED
    void MeshRenderer::EditorGuiElement()
    {
        ImGui::Text("Mesh Renderer");
        m_material.EditorGuiElement();
    }
    #endif
}