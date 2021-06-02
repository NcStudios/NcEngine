#pragma once

#include "Ecs.h"

namespace nc::sample
{
    class ConstantTranslation : public AutoComponent
    {
        public:
            ConstantTranslation(Entity entity, registry_type* registry, Vector3 velocity);
            void FrameUpdate(float dt) override;
        
        private:
            registry_type* m_registry;
            Vector3 m_velocity;
    };

    inline ConstantTranslation::ConstantTranslation(Entity entity, registry_type* registry, Vector3 velocity)
        : AutoComponent{entity},
          m_registry{registry},
          m_velocity{velocity}
    {
    }

    inline void ConstantTranslation::FrameUpdate(float dt)
    {
        m_registry->Get<Transform>(GetParentEntity())->Translate(m_velocity * dt);
    }
}