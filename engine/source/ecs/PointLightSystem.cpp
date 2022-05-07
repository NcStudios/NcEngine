#include "PointLightSystem.h"
#include "ecs/Registry.h"
#include "ecs/component/PointLight.h"

namespace nc::ecs
{
    PointLightSystem::PointLightSystem(Registry* registry)
        : m_registry{registry},
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