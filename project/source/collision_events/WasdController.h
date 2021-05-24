#pragma once

#include "Ecs.h"
#include "Input.h"

namespace nc::sample
{
    class WasdController : public Component
    {
        public:
            WasdController(EntityHandle parentHandle, float speed);
            void FrameUpdate(float dt) override;

        private:
            //Transform* m_transform;
            float m_speed;
    };

    inline WasdController::WasdController(EntityHandle parentHandle, float speed)
        : Component{parentHandle},
            //m_transform{GetComponent<Transform>(parentHandle)},
            m_speed{speed}
    {
    }
    
    inline void WasdController::FrameUpdate(float dt)
    {
        auto [leftRight, frontBack] = input::GetAxis() * m_speed * dt;
        GetComponent<Transform>(GetParentHandle())->TranslateLocalSpace(Vector3{leftRight, 0.0f, frontBack});
    }
}