#pragma once

#include "Ecs.h"

namespace nc::sample
{
    class ConstantTranslation : public Component
    {
        public:
            ConstantTranslation(EntityHandle handle, Vector3 velocity);
            void FrameUpdate(float dt) override;
        
        private:
            Transform* m_transform;
            Vector3 m_velocity;
    };

    inline ConstantTranslation::ConstantTranslation(EntityHandle handle, Vector3 velocity)
        : Component{handle},
            m_transform{GetComponent<Transform>(handle)},
            m_velocity{velocity}
    {
    }

    inline void ConstantTranslation::FrameUpdate(float dt)
    {
        m_transform->Translate(m_velocity * dt, Space::World);
    }
}