#include "PointLightSystem.h"
#include "ecs/Registry.h"
#include "graphics/PointLight.h"

namespace nc::ecs
{
    PointLightSystem::PointLightSystem(Registry* registry)
        : m_onAddConnection{registry->OnAdd<graphics::PointLight>().Connect([this](graphics::PointLight&){ this->m_isSystemDirty = true; })},
          m_onRemoveConnection{registry->OnRemove<graphics::PointLight>().Connect([this](Entity){ this->m_isSystemDirty = true; })},
          m_isSystemDirty{true}
    {
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