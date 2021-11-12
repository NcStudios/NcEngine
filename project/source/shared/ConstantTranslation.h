#pragma once

#include "ecs/Registry.h"

namespace nc::sample
{
    class ConstantTranslation : public AutoComponent
    {
        public:
            ConstantTranslation(Entity entity, Registry* registry, Vector3 velocity);
            void FrameUpdate(float dt) override;
        
        private:
            Registry* m_registry;
            Vector3 m_velocity;
    };

    inline ConstantTranslation::ConstantTranslation(Entity entity, Registry* registry, Vector3 velocity)
        : AutoComponent{entity},
          m_registry{registry},
          m_velocity{velocity}
    {
    }

    inline void ConstantTranslation::FrameUpdate(float dt)
    {
        m_registry->Get<Transform>(ParentEntity())->Translate(m_velocity * dt);
    }
}