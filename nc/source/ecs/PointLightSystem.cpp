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
    : m_graphics{graphics},
      m_registry{registry},
      m_isSystemDirty{true}
    {
        m_registry->RegisterOnAddCallback<vulkan::PointLight>
        (
            [this](vulkan::PointLight&) { m_isSystemDirty = true; }
        );

        m_registry->RegisterOnRemoveCallback<vulkan::PointLight>
        (
            [this](Entity) { m_isSystemDirty = true; }
        );

        graphics::vulkan::ResourceManager::InitializePointLights(graphics, MAX_POINT_LIGHTS);
    }

    void PointLightSystem::Update()
    {
        auto pointLightComponents = m_registry->ViewAll<vulkan::PointLight>();
        for (auto& pointLight : pointLightComponents)
        {
            if (pointLight.Update())
            {
                m_isSystemDirty = true;
            }
        }

        if (m_isSystemDirty)
        {
            auto pointLightInfos = std::vector<vulkan::PointLightInfo>();
            pointLightInfos.reserve(pointLightComponents.size());

            // Pull the PointLightInfo structs out of their respective components in the registry to map them to the GPU.
            std::transform(pointLightComponents.begin(), pointLightComponents.end(), std::back_inserter(pointLightInfos), [](auto&& pointLightComponent)
            { 
                return pointLightComponent.GetInfo(); 
            });

            graphics::vulkan::ResourceManager::UpdatePointLights(pointLightInfos);
            m_isSystemDirty = false;
        }
    }

    void PointLightSystem::Clear()
    {
        m_isSystemDirty = true;
        graphics::vulkan::ResourceManager::ResetPointLights(m_graphics, MAX_POINT_LIGHTS);
    }
}