#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/Renderer.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(graphics::Graphics2* graphics)
    : m_graphics{graphics}
    {
    }

    void MeshRendererSystem::Add(vulkan::MeshRenderer& meshRenderer)
    {
        auto techniqueType = meshRenderer.GetTechniqueType();
        m_graphics->GetRendererPtr()->RegisterMeshRenderer(techniqueType, &meshRenderer);
    }

    void MeshRendererSystem::Remove(EntityHandle)
    {
        //@todo
    }
}