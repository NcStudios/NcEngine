#pragma once

#include "Ecs.h"
#include "shared/GameLog.h"



#include "Input.h"

namespace nc::sample
{
    /** Destroys anything that escapes the area bounded by the parent entity's collider. */
    class KillBox : public Component
    {
        public:
            KillBox(EntityHandle handle, bool logOnDestroy = false);
            void OnCollisionExit(Entity* other) override;

            void FrameUpdate(float dt) override
            {
                if(input::GetKeyDown(input::KeyCode::A))
                    m_particleSystem->Emit(10u);
            }

        private:
            bool m_logOnDestroy;
            ParticleEmitter* m_particleSystem;
    };

    inline KillBox::KillBox(EntityHandle handle, bool logOnDestroy)
        : Component{handle},
          m_logOnDestroy{logOnDestroy},
          m_particleSystem{AddComponent<ParticleEmitter>(handle, ParticleInfo{})}
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