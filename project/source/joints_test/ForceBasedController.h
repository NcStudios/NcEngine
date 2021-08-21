#pragma once

#include "Ecs.h"
#include "Input.h"

namespace nc::sample
{
    class ForceBasedController : public AutoComponent
    {
        public:
            ForceBasedController(Entity entity, registry_type* registry);
            void FrameUpdate(float dt) override;

        private:
            registry_type* m_registry;
    };

    inline ForceBasedController::ForceBasedController(Entity entity, registry_type* registry)
        : AutoComponent{entity},
          m_registry{registry}
    {
    }

    inline void ForceBasedController::FrameUpdate(float)
    {
        auto* body = m_registry->Get<PhysicsBody>(GetParentEntity());

        if(!body)
            return;

        constexpr auto move = Vector3{0.0f, 0.0f, 0.7f};
        constexpr auto sidestep = Vector3{0.7f, 0.0f, 0.0f};
        constexpr auto jump = Vector3{0.0f, 4.0f, 0.0f};
        constexpr auto rotate = Vector3{0.0f, 0.6f, 0.0f};

        if(input::GetKey(input::KeyCode::W))
            body->ApplyImpulse(move);

        if(input::GetKey(input::KeyCode::S))
            body->ApplyImpulse(-move);

        if(input::GetKey(input::KeyCode::A))
            body->ApplyImpulse(-sidestep);
        
        if(input::GetKey(input::KeyCode::D))
            body->ApplyImpulse(sidestep);

        if(input::GetKey(input::KeyCode::Space))
            body->ApplyImpulse(jump);

        if(input::GetKey(input::KeyCode::Q))
            body->ApplyTorqueImpulse(-rotate);

        if(input::GetKey(input::KeyCode::E))
            body->ApplyTorqueImpulse(rotate);
    }
}