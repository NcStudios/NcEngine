#include "PointLightSystem.h"
#include "ECS.h"
#include "EntityComponentSystem.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"

namespace nc::ecs
{
    const uint32_t PointLightSystem::MAX_POINT_LIGHTS;

    PointLightSystem::PointLightSystem(registry_type* registry, graphics::Graphics2* graphics, uint32_t maxPointLights)
    : m_pointLightInfos{},
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

        graphics::vulkan::ResourceManager::InitializePointLights(graphics, maxPointLights);
    }

    void PointLightSystem::Update()
    {
        m_pointLightInfos.clear();
        for (auto& pointLight : m_registry->ViewAll<vulkan::PointLight>())
        {
            pointLight.Update();
            m_pointLightInfos.push_back(pointLight.GetInfo());
        }
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }


    void PointLightSystem::Add(vulkan::PointLight& pointLight)
    {
        auto& info = pointLight.GetInfo();
        m_pointLightInfos.push_back(info);
        graphics::vulkan::ResourceManager::UpdatePointLights(m_pointLightInfos);
    }

    void PointLightSystem::Remove(Entity)
    {
    }

    void PointLightSystem::Clear()
    {
        m_pointLightInfos.resize(0);
    }
}