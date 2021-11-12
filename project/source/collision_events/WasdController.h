#pragma once

#include "ecs/Registry.h"
#include "Input.h"

#include <iostream>

namespace nc::sample
{
    class WasdController : public AutoComponent
    {
        public:
            WasdController(Entity entity, Registry* registry, float speed);
            void FrameUpdate(float dt) override;

        private:
            Registry* m_registry;
            float m_speed;
    };

    inline WasdController::WasdController(Entity entity, Registry* registry, float speed)
        : AutoComponent{entity},
          m_registry{registry},
          m_speed{speed}
    {
    }
    
    inline void WasdController::FrameUpdate(float dt)
    {
        auto [leftRight, frontBack] = input::GetAxis() * m_speed * dt;

        auto q = input::GetKey(input::KeyCode::Q);
        auto e = input::GetKey(input::KeyCode::E);
        auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * m_speed * dt;

        m_registry->Get<Transform>(ParentEntity())->Translate(Vector3{leftRight, upDown, frontBack});

        if (input::GetKeyDown(input::KeyCode::B))
        {
            std::cout << "Remove pressed.\n";
            m_registry->Remove<PointLight>(ParentEntity());
        }
        else if (input::GetKeyDown(input::KeyCode::V))
        {
            std::cout << "Add pressed.\n";
            m_registry->Add<PointLight>(ParentEntity(), PointLightInfo{});
        }
    }
}