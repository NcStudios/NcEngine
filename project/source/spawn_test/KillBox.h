#pragma once

#include "Ecs.h"
#include "shared/GameLog.h"

namespace nc::sample
{
    /** Destroys anything that escapes the area bounded by the parent entity's collider. */
    class KillBox : public Component
    {
        public:
            KillBox(EntityHandle handle, bool logOnDestroy = false);
            void OnCollisionExit(EntityHandle hit) override;

        private:
            bool m_logOnDestroy;
    };

    inline KillBox::KillBox(EntityHandle handle, bool logOnDestroy)
        : Component{handle},
          m_logOnDestroy{logOnDestroy}
    {
    }

    inline void KillBox::OnCollisionExit(EntityHandle hit)
    {
        if(m_logOnDestroy)
            GameLog::Log("KillBox - Object Destroyed");
        DestroyEntity(hit);
    }
}