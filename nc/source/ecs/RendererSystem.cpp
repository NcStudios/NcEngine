#include "RendererSystem.h"
#include "ECS.h"

namespace nc::ecs
{
    RendererSystem::RendererSystem(uint32_t renderersCount)
    : m_componentSystem{renderersCount}
    {
        internal::RegisterRendererSystem(this);
    }

    ComponentSystem<Renderer2>* RendererSystem::GetSystem()
    {
        return &m_componentSystem;
    }

    Renderer2* RendererSystem::Add(EntityHandle parentHandle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType)
    {
        (void)meshUid;
        (void)techniqueType;
        return m_componentSystem.Add(parentHandle);
    }

    bool RendererSystem::Remove(EntityHandle parentHandle)
    {
        return m_componentSystem.Remove(parentHandle);
    }

    bool RendererSystem::Contains(EntityHandle parentHandle) const
    {
        return m_componentSystem.Contains(parentHandle);
    }

    Renderer2* RendererSystem::GetPointerTo(EntityHandle parentHandle)
    {
        return m_componentSystem.GetPointerTo(parentHandle);
    }

    auto RendererSystem::GetComponents() -> ComponentSystem<Renderer2>::ContainerType&
    {
        return m_componentSystem.GetComponents();
    }

    void RendererSystem::Clear()
    {
        m_componentSystem.Clear();
    }
}