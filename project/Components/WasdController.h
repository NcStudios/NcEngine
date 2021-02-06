#pragma once

#include "Ecs.h"
#include "Input.h"

namespace project
{
    class WasdController : public nc::Component
    {
        public:
            WasdController(nc::EntityHandle parentHandle, float speed)
                : Component{parentHandle},
                  m_transform{nc::GetComponent<nc::Transform>(parentHandle)},
                  m_speed{speed}
            {
            }

            void FrameUpdate(float dt) override
            {
                auto [leftRight, frontBack] = nc::input::GetAxis() * m_speed * dt;
                m_transform->Translate(nc::Vector3{leftRight, 0.0f, frontBack}, nc::Space::Local);
            }

        private:
            nc::Transform* m_transform;
            float m_speed;
    };
}