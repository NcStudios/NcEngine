#pragma once

#include "Ecs.h"

namespace nc::sample
{
    class SoundPlayer : public AutoComponent
    {
        public:
            SoundPlayer(Entity entity, registry_type* registry)
                : AutoComponent{entity},
                  m_registry{registry}
            {
            }
        
            void OnCollisionEnter(Entity) override
            {
                //if(auto* audio = m_registry->Get<AudioSource>(GetParentEntity()); audio)
               // {
               //     audio->Play();
               // }
            }

        private:
            registry_type* m_registry;
    };
}