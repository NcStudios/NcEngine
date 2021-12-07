#pragma once

#include "ecs/Registry.h"
#include "ecs/component/PhysicsBody.h"
#include "Input.h"

namespace nc::sample
{
    class ForceBasedController : public AutoComponent
    {
        public:
            ForceBasedController(Entity entity, Registry* registry, float force = 0.7f);
            void FixedUpdate() override;

        private:
            Registry* m_registry;
            float m_force;
    };

    inline ForceBasedController::ForceBasedController(Entity entity, Registry* registry, float force)
        : AutoComponent{entity},
          m_registry{registry},
          m_force{force}
    {
    }

    inline void ForceBasedController::FixedUpdate()
    {
        auto* body = m_registry->Get<PhysicsBody>(ParentEntity());

        if(!body)
            return;

        if(input::GetKey(input::KeyCode::W))
            body->ApplyImpulse(Vector3::Front() * m_force);

        if(input::GetKey(input::KeyCode::S))
            body->ApplyImpulse(Vector3::Back() * m_force);

        if(input::GetKey(input::KeyCode::A))
            body->ApplyImpulse(Vector3::Left() * m_force);
        
        if(input::GetKey(input::KeyCode::D))
            body->ApplyImpulse(Vector3::Right() * m_force);

        if(input::GetKey(input::KeyCode::Space))
            body->ApplyImpulse(Vector3::Up() * 4.0f);

        if(input::GetKey(input::KeyCode::Q))
            body->ApplyTorqueImpulse(Vector3::Down() * 0.6f);

        if(input::GetKey(input::KeyCode::E))
            body->ApplyTorqueImpulse(Vector3::Up() * 0.6f);
    }
}