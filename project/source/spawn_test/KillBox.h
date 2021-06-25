#pragma once

#include "Ecs.h"
#include "shared/GameLog.h"

namespace nc::sample
{
    /** Destroys anything that escapes the area bounded by the parent entity's collider. */
    class KillBox : public AutoComponent
    {
        public:
            KillBox(Entity entity, registry_type* registry, bool logOnDestroy = false);
            void OnCollisionExit(Entity hit) override;

        private:
            registry_type* m_registry;
            bool m_logOnDestroy;
    };

    inline KillBox::KillBox(Entity entity, registry_type* registry, bool logOnDestroy)
        : AutoComponent{entity},
          m_registry{registry},
          m_logOnDestroy{logOnDestroy}
    {
    }

    inline void KillBox::OnCollisionExit(Entity hit)
    {
        if(m_logOnDestroy)
            GameLog::Log("KillBox - Object Destroyed");
        m_registry->Remove<Entity>(hit);
    }
}