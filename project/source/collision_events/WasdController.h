#pragma once

#include "Ecs.h"
#include "Input.h"

namespace nc::sample
{
    class WasdController : public AutoComponent
    {
        public:
            WasdController(Entity entity, registry_type* registry, float speed);
            void FrameUpdate(float dt) override;

        private:
            registry_type* m_registry;
            float m_speed;
    };

    inline WasdController::WasdController(Entity entity, registry_type* registry, float speed)
        : AutoComponent{entity},
          m_registry{registry},
          m_speed{speed}
    {
    }
    
    inline void WasdController::FrameUpdate(float dt)
    {
        auto [leftRight, frontBack] = input::GetAxis() * m_speed * dt;
        m_registry->Get<Transform>(GetParentEntity())->TranslateLocalSpace(Vector3{leftRight, 0.0f, frontBack});
    }
}