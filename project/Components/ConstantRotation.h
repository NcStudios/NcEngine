#pragma once
#include "Ecs.h"

namespace project
{
    /** Applies a constant rotation around an axis to the parent object */
    class ConstantRotation : public nc::Component
    {
        public:
            ConstantRotation(nc::EntityHandle handle, nc::Vector3 axis, float radiansPerSecond)
                : Component{handle},
                  m_transform{GetComponent<nc::Transform>(handle)},
                  m_axis{axis},
                  m_radiansPerSecond{radiansPerSecond}
            {
            }

            void FrameUpdate(float dt) override
            {
                m_transform->Rotate(m_axis, m_radiansPerSecond * dt);
            }

        private:
            nc::Transform* m_transform;
            nc::Vector3 m_axis;
            float m_radiansPerSecond;
    };
}