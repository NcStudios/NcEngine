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
            void OnCollisionExit(Entity* other) override;

        private:
            bool m_logOnDestroy;
            #ifndef USE_VULKAN
            ParticleEmitter* m_particleSystem;
            #endif
    };

    inline KillBox::KillBox(EntityHandle handle, bool logOnDestroy)
        : Component{handle},
          m_logOnDestroy{logOnDestroy}
          #ifndef USE_VULKAN
          ,
          m_particleSystem{AddComponent<ParticleEmitter>(handle, ParticleInfo{})}
          #endif
    {
    }

    inline void KillBox::OnCollisionExit(Entity* other)
    {
        if(other)
        {
            if(m_logOnDestroy)
                GameLog::Log("KillBox - Object Destroyed");
            DestroyEntity(other->Handle);
        }
    }
}