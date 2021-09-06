#include "PointLightSystem.h"
#include "ECS.h"
#include "EntityComponentSystem.h"
#include "graphics/Graphics2.h"
#include "graphics/Commands.h"
#include "graphics/Renderer.h"
#include "graphics/resources/ResourceManager.h"

#include <iostream>

namespace nc::ecs
{
    const uint32_t PointLightSystem::MAX_POINT_LIGHTS;

    PointLightSystem::PointLightSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_registry{registry},
      m_isSystemDirty{true}
    {
        m_registry->RegisterOnAddCallback<PointLight>
        (
            [this](PointLight&) { m_isSystemDirty = true; }
        );

        m_registry->RegisterOnRemoveCallback<PointLight>
        (
            [this](Entity) { m_isSystemDirty = true; }
        );

        graphics::ResourceManager::InitializePointLights(graphics, MAX_POINT_LIGHTS);
    }

    void PointLightSystem::Update()
    {
        auto pointLightComponents = m_registry->ViewAll<PointLight>();
        const auto& view = m_graphics->GetViewMatrix();
        for (auto& pointLight : pointLightComponents)
        {
            if (pointLight.Update(view))
            {
                m_isSystemDirty = true;
            }
        }

        if (m_isSystemDirty)
        {
            auto pointLightInfos = std::vector<PointLightInfo>();
            pointLightInfos.reserve(pointLightComponents.size());

            // Pull the PointLightInfo structs out of their respective components in the registry to map them to the GPU.
            std::transform(pointLightComponents.begin(), pointLightComponents.end(), std::back_inserter(pointLightInfos), [](auto&& pointLightComponent)
            { 
                return pointLightComponent.GetInfo(); 
            });

            graphics::ResourceManager::UpdatePointLights(pointLightInfos);
            m_isSystemDirty = false;
        }
    }

    void PointLightSystem::Clear()
    {
        m_isSystemDirty = true;
        graphics::ResourceManager::ResetPointLights(m_graphics, MAX_POINT_LIGHTS);
    }
}