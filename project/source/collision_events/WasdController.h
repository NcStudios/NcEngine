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

        auto q = input::GetKey(input::KeyCode::Q);
        auto e = input::GetKey(input::KeyCode::E);
        auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * m_speed * dt;

        m_registry->Get<Transform>(GetParentEntity())->Translate(Vector3{leftRight, upDown, frontBack});
        //m_registry->Get<Transform>(GetParentEntity())->TranslateLocalSpace(Vector3{leftRight, 0.0f, frontBack});
    }

    class WasdController2 : public AutoComponent
    {
        public:
            WasdController2(Entity entity, registry_type* registry, float speed);
            void FrameUpdate(float dt) override;

        private:
            registry_type* m_registry;
            float m_speed;
    };

    inline WasdController2::WasdController2(Entity entity, registry_type* registry, float speed)
        : AutoComponent{entity},
          m_registry{registry},
          m_speed{speed}
    {
    }
    
    inline void WasdController2::FrameUpdate(float dt)
    {
        //auto [leftRight, frontBack] = input::GetAxis() * m_speed * dt;

        auto j = input::GetKey(input::KeyCode::J);
        auto l = input::GetKey(input::KeyCode::L);
        auto i = input::GetKey(input::KeyCode::I);
        auto k = input::GetKey(input::KeyCode::K);

        auto leftRight = (static_cast<float>(l) - static_cast<float>(j)) * m_speed * dt;
        auto frontBack = (static_cast<float>(i) - static_cast<float>(k)) * m_speed * dt;

        //auto q = input::GetKey(input::KeyCode::Q);
        //auto e = input::GetKey(input::KeyCode::E);
        //auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * m_speed * dt;

        m_registry->Get<Transform>(GetParentEntity())->Translate(Vector3{leftRight, 0.0f, frontBack});
        //m_registry->Get<Transform>(GetParentEntity())->TranslateLocalSpace(Vector3{leftRight, 0.0f, frontBack});
    }
}