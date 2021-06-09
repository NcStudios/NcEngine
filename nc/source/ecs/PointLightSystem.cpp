#include "PointLightSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"

namespace nc::ecs
{
    const uint32_t PointLightSystem::MAX_POINT_LIGHTS;

    PointLightSystem::PointLightSystem(graphics::Graphics2* graphics, uint32_t maxPointLights)
    : m_componentSystem{maxPointLights},
      m_pointLightInfos{},
      m_graphics{graphics}
    {
        graphics::vulkan::ResourceManager::InitializePointLights(graphics, maxPointLights);
    }

    ComponentSystem<vulkan::PointLight>* PointLightSystem::GetSystem()
    {
        return &m_componentSystem;
    }

    vulkan::PointLight* PointLightSystem::Add(EntityHandle parentHandle, vulkan::PointLightInfo info)
    {
        auto pointLight = m_componentSystem.Add(parentHandle, info);
        info.isInitialized = true;
        m_pointLightInfos.push_back(info);
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
        return pointLight;
    }

    void PointLightSystem::Update()
    {
        m_pointLightInfos.clear();
        for (auto& pointLight : m_componentSystem.GetComponents())
        {
            pointLight->Update();
            m_pointLightInfos.push_back(pointLight->GetInfo());
        }
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }

    bool PointLightSystem::Remove(EntityHandle parentHandle)
    {
        return m_componentSystem.Remove(parentHandle);
    }

    bool PointLightSystem::Contains(EntityHandle parentHandle) const
    {
        return m_componentSystem.Contains(parentHandle);
    }

    vulkan::PointLight* PointLightSystem::GetPointerTo(EntityHandle parentHandle)
    {
        return m_componentSystem.GetPointerTo(parentHandle);
    }

    auto PointLightSystem::GetComponents() -> ComponentSystem<vulkan::PointLight>::ContainerType&
    {
        return m_componentSystem.GetComponents();
    }

    void PointLightSystem::Clear()
    {
        m_pointLightInfos.resize(0);
        m_componentSystem.Clear();
    }
}