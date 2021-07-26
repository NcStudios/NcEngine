#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/Renderer.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_graphics{graphics}
    {
        registry->RegisterOnAddCallback<vulkan::MeshRenderer>
        (
            [this](vulkan::MeshRenderer& meshRenderer) { this->Add(meshRenderer); }
        );

        registry->RegisterOnRemoveCallback<vulkan::MeshRenderer>
        (
            [this](Entity entity) { this->Remove(entity); }
        );
    }

    void MeshRendererSystem::Add(vulkan::MeshRenderer& meshRenderer)
    {
        auto techniqueType = meshRenderer.GetTechniqueType();
        m_graphics->GetRendererPtr()->RegisterMeshRenderer(techniqueType, &meshRenderer);
    }

    void MeshRendererSystem::Remove(Entity entity)
    {
        m_graphics->GetRendererPtr()->DeregisterRenderable(entity);
    }

    void MeshRendererSystem::Clear()
    {
        m_graphics->GetRendererPtr()->ClearMeshRenderers();
    }
}