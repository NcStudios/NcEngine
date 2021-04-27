#include "RendererSystem.h"
#include "ECS.h"
#include "graphics/vulkan/Commands.h"

namespace nc::ecs
{
    RendererSystem::RendererSystem(uint32_t renderersCount)
    : m_componentSystem{renderersCount},
      m_meshManager{},
      m_techniqueManager{}
    {
        internal::RegisterRendererSystem(this);
        m_techniqueManager.RegisterGlobalData(m_meshManager.GetVertexBuffer(), m_meshManager.GetIndexBuffer());
    }

    ComponentSystem<vulkan::Renderer>* RendererSystem::GetSystem()
    {
        return &m_componentSystem;
    }

    void RendererSystem::RecordTechniques(graphics::vulkan::Commands* commands)
    {
        m_techniqueManager.RecordTechniques(commands);
    }

    vulkan::Renderer* RendererSystem::Add(EntityHandle parentHandle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType)
    {
        (void)meshUid;
        (void)techniqueType;

        auto mesh = m_meshManager.GetMesh(std::move(meshUid));
        auto renderer = m_componentSystem.Add(parentHandle);
        m_techniqueManager.RegisterRenderer(techniqueType, std::move(mesh), renderer->GetTransform());
        return renderer;
    }

    bool RendererSystem::Remove(EntityHandle parentHandle)
    {
        return m_componentSystem.Remove(parentHandle);
    }

    bool RendererSystem::Contains(EntityHandle parentHandle) const
    {
        return m_componentSystem.Contains(parentHandle);
    }

    vulkan::Renderer* RendererSystem::GetPointerTo(EntityHandle parentHandle)
    {
        return m_componentSystem.GetPointerTo(parentHandle);
    }

    auto RendererSystem::GetComponents() -> ComponentSystem<vulkan::Renderer>::ContainerType&
    {
        return m_componentSystem.GetComponents();
    }

    void RendererSystem::Clear()
    {
        m_meshManager.Clear();
        m_techniqueManager.Clear();
        m_componentSystem.Clear();
    }
}