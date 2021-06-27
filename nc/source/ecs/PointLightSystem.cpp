#include "PointLightSystem.h"
#include "ECS.h"
#include "EntityComponentSystem.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"

#include <iostream>

namespace nc::ecs
{
    const uint32_t PointLightSystem::MAX_POINT_LIGHTS;

    PointLightSystem::PointLightSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_pointLightEntities{},
      m_pointLightInfos{},
      m_graphics{graphics},
      m_registry{registry}
    {
        m_registry->RegisterOnAddCallback<vulkan::PointLight>
        (
            [this](vulkan::PointLight& pointLight) { this->Add(pointLight); }
        );

        m_registry->RegisterOnRemoveCallback<vulkan::PointLight>
        (
            [this](Entity entity) { this->Remove(entity); }
        );

        graphics::vulkan::ResourceManager::InitializePointLights(graphics, MAX_POINT_LIGHTS);
    }

    void PointLightSystem::Update()
    {
        m_pointLightInfos.clear();
        m_pointLightEntities.clear();

        for (auto& pointLight : m_registry->ViewAll<vulkan::PointLight>())
        {
            pointLight.Update();
            m_pointLightInfos.emplace_back(pointLight.GetInfo());
            m_pointLightEntities.emplace_back(pointLight.GetParentEntity());
        }
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }

    void PointLightSystem::Add(vulkan::PointLight& pointLight)
    {
        m_pointLightInfos.emplace_back(pointLight.GetInfo());
        m_pointLightEntities.emplace_back(pointLight.GetParentEntity());
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }

    void PointLightSystem::Remove(Entity entity)
    {
        auto entityIt = std::ranges::find(m_pointLightEntities, entity);

        if (entityIt == m_pointLightEntities.end())
        {
            return;
        }

        auto index = static_cast<uint32_t>(entityIt - m_pointLightEntities.begin());
        auto& pointLight = m_pointLightInfos[index];
        pointLight.isInitialized = false;
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }

    void PointLightSystem::Clear()
    {
        m_pointLightInfos.clear();
        m_pointLightEntities.clear();
        graphics::vulkan::ResourceManager::ResetPointLights(m_graphics, MAX_POINT_LIGHTS);
    }
}