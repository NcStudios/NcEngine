#pragma once

#include "Ecs.h"

namespace nc::sample
{
    /** Applies a constant rotation around an axis to the parent object */
    class ConstantRotation : public Component
    {
        public:
            ConstantRotation(EntityHandle handle, Vector3 axis, float radiansPerSecond);
            void FrameUpdate(float dt) override;

        private:
            //Transform* m_transform;
            Vector3 m_axis;
            float m_radiansPerSecond;
    };

    inline ConstantRotation::ConstantRotation(EntityHandle handle, Vector3 axis, float radiansPerSecond)
        : Component{handle},
          //m_transform{GetComponent<Transform>(handle)},
          m_axis{axis},
          m_radiansPerSecond{radiansPerSecond}
    {
    }

    inline void ConstantRotation::FrameUpdate(float dt)
    {
        GetComponent<Transform>(GetParentHandle())->Rotate(m_axis, m_radiansPerSecond * dt);
    }
}