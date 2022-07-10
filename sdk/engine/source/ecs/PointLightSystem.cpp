#include "PointLightSystem.h"
#include "ecs/Registry.h"
#include "ecs/component/PointLight.h"

namespace nc::ecs
{
    PointLightSystem::PointLightSystem(Registry* registry)
        : m_onAddConnection{registry->OnAdd<PointLight>().Connect([this](PointLight&){ this->m_isSystemDirty = true; })},
          m_onRemoveConnection{registry->OnRemove<PointLight>().Connect([this](Entity){ this->m_isSystemDirty = true; })},
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