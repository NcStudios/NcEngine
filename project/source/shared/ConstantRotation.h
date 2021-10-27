#pragma once

#include "ecs/Registry.h"

namespace nc::sample
{
    /** Applies a constant rotation around an axis to the parent object */
    class ConstantRotation : public AutoComponent
    {
        public:
            ConstantRotation(Entity entity, Registry* registry, Vector3 axis, float radiansPerSecond);
            void FrameUpdate(float dt) override;

        private:
            Registry* m_registry;
            Vector3 m_axis;
            float m_radiansPerSecond;
    };

    inline ConstantRotation::ConstantRotation(Entity entity, Registry* registry, Vector3 axis, float radiansPerSecond)
        : AutoComponent{entity},
          m_registry{registry},
          m_axis{axis},
          m_radiansPerSecond{radiansPerSecond}
    {
    }

    inline void ConstantRotation::FrameUpdate(float dt)
    {
        m_registry->Get<Transform>(GetParentEntity())->Rotate(m_axis, m_radiansPerSecond * dt);
    }
}