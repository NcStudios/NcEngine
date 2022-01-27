#pragma once

#include "ecs/Registry.h"
#include "shared/GameLog.h"

namespace nc::sample
{
    /** Destroys anything that escapes the area bounded by the parent entity's collider. */
    class KillBox : public AutoComponent
    {
        public:
            KillBox(Entity entity, Registry* registry, bool logOnDestroy = false);
            void OnTriggerExit(Entity hit) override;

        private:
            Registry* m_registry;
            bool m_logOnDestroy;
    };

    inline KillBox::KillBox(Entity entity, Registry* registry, bool logOnDestroy)
        : AutoComponent{entity},
          m_registry{registry},
          m_logOnDestroy{logOnDestroy}
    {
    }

    inline void KillBox::OnTriggerExit(Entity hit)
    {
        if(m_logOnDestroy)
            GameLog::Log("KillBox - Object Destroyed");

        if(m_registry->Contains<Entity>(hit))
            m_registry->Remove<Entity>(hit);
    }
}
