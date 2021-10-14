#include "PointLightSystem.h"
#include "ECS.h"
#include "EntityComponentSystem.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"

#include <iostream>

namespace nc::ecs
{
    PointLightSystem::PointLightSystem(registry_type* registry, graphics::Graphics* graphics)
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
    }

    bool PointLightSystem::CheckDirtyAndReset()
    {
        return std::exchange(m_isSystemDirty, false);
    }

    void PointLightSystem::Clear()
    {
        m_isSystemDirty = true;
    }
}