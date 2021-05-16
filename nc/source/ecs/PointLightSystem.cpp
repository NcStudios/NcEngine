#include "PointLightSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/ResourceManager.h"

namespace nc::ecs
{
    const uint32_t PointLightSystem::MAX_POINT_LIGHTS;

    PointLightSystem::PointLightSystem(graphics::Graphics2* graphics, uint32_t maxPointLights)
    : m_componentSystem{maxPointLights},
      m_graphics{graphics}
    {
    }

    ComponentSystem<vulkan::PointLight>* PointLightSystem::GetSystem()
    {
        return &m_componentSystem;
    }

    vulkan::PointLight* PointLightSystem::Add(EntityHandle parentHandle, vulkan::PointLightInfo info)
    {
        auto renderer = m_componentSystem.Add(parentHandle, info);
        ResourceManager::LoadPointLight(info);
        m_graphics->GetRendererPtr()->RegisterMeshRenderer(techniqueType, renderer);

        m_techniqueManager.RegisterMeshRenderer(techniqueType, renderer);
        return renderer;
    }

    vulkan::PointLight* PointLightSystem::Add(EntityHandle parentHandle)
    {
        auto renderer = m_componentSystem.Add(parentHandle, std::move(meshUid), std::move(material), GetComponent<Transform>(parentHandle));
        m_graphics->GetRendererPtr()->RegisterMeshRenderer(techniqueType, renderer);
        m_techniqueManager.RegisterMeshRenderer(techniqueType, renderer);
        return renderer;
    }

    bool PointLightSystem::Remove(EntityHandle parentHandle)
    {
        return m_componentSystem.Remove(parentHandle);
    }

    bool PointLightSystem::Contains(EntityHandle parentHandle) const
    {
        return m_componentSystem.Contains(parentHandle);
    }

    vulkan::MeshRenderer* PointLightSystem::GetPointerTo(EntityHandle parentHandle)
    {
        return m_componentSystem.GetPointerTo(parentHandle);
    }

    auto PointLightSystem::GetComponents() -> ComponentSystem<vulkan::MeshRenderer>::ContainerType&
    {
        return m_componentSystem.GetComponents();
    }

    void PointLightSystem::Clear()
    {
        m_techniqueManager.Clear();
        m_componentSystem.Clear();
    }
}