#include "component/vulkan/MeshRenderer.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    MeshRenderer::MeshRenderer(Entity entity, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType)
    : ComponentBase{entity},
      m_meshUid{std::move(meshUid)},
      m_material{std::move(material)},
      m_techniqueType{techniqueType}
    {
    }

    Transform* MeshRenderer::GetTransform()
    {
        return ActiveRegistry()->Get<Transform>(GetParentEntity());
    }

    const std::string& MeshRenderer::GetMeshUid() const
    {
        return m_meshUid;
    }

    nc::graphics::vulkan::Material& MeshRenderer::GetMaterial()
    {
        return m_material;
    }

    nc::graphics::vulkan::TechniqueType MeshRenderer::GetTechniqueType() const
    {
        return m_techniqueType;
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