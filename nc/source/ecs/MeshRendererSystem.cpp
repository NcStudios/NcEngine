#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/Renderer.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(uint32_t renderersCount, graphics::Graphics2* graphics)
    : m_componentSystem{renderersCount},
      m_graphics{graphics}
    {
    }

    ComponentSystem<vulkan::MeshRenderer>* MeshRendererSystem::GetSystem()
    {
        return &m_componentSystem;
    }

    vulkan::MeshRenderer* MeshRendererSystem::Add(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType)
    {
        auto renderer = m_componentSystem.Add(parentHandle, std::move(meshUid), std::move(material), GetComponent<Transform>(parentHandle));
        m_graphics->GetRendererPtr()->RegisterMeshRenderer(techniqueType, renderer);
        return renderer;
    }

    bool MeshRendererSystem::Remove(EntityHandle parentHandle)
    {
        return m_componentSystem.Remove(parentHandle);
    }

    bool MeshRendererSystem::Contains(EntityHandle parentHandle) const
    {
        return m_componentSystem.Contains(parentHandle);
    }

    vulkan::MeshRenderer* MeshRendererSystem::GetPointerTo(EntityHandle parentHandle)
    {
        return m_componentSystem.GetPointerTo(parentHandle);
    }

    auto MeshRendererSystem::GetComponents() -> ComponentSystem<vulkan::MeshRenderer>::ContainerType&
    {
        return m_componentSystem.GetComponents();
    }

    void MeshRendererSystem::Clear()
    {
        m_componentSystem.Clear();
    }
}