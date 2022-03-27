#include "GameLogic.h"
#include "GameLog.h"
#include "Input.h"

namespace nc::sample
{
    void WasdBasedMovement(Entity self, Registry* registry, float dt)
    {
        static constexpr float speed = 2.0f;
        auto [leftRight, frontBack] = input::GetAxis() * speed * dt;
        auto q = input::GetKey(input::KeyCode::Q);
        auto e = input::GetKey(input::KeyCode::E);
        auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * speed * dt;
        registry->Get<Transform>(self)->Translate(Vector3{leftRight, upDown, frontBack});
    }

    void ForceBasedMovement(Entity self, Registry* registry)
    {
        static constexpr float force = 0.7f;
        auto* body = registry->Get<PhysicsBody>(self);

        if(!body)
            return;

        if(input::GetKey(input::KeyCode::W))
            body->ApplyImpulse(Vector3::Front() * force);

        if(input::GetKey(input::KeyCode::S))
            body->ApplyImpulse(Vector3::Back() * force);

        if(input::GetKey(input::KeyCode::A))
            body->ApplyImpulse(Vector3::Left() * force);
        
        if(input::GetKey(input::KeyCode::D))
            body->ApplyImpulse(Vector3::Right() * force);

        if(input::GetKey(input::KeyCode::Space))
            body->ApplyImpulse(Vector3::Up() * 4.0f);

        if(input::GetKey(input::KeyCode::Q))
            body->ApplyTorqueImpulse(Vector3::Down() * 0.6f);

        if(input::GetKey(input::KeyCode::E))
            body->ApplyTorqueImpulse(Vector3::Up() * 0.6f);
    }

    void DestroyOnTriggerExit(Entity, Entity hit, Registry* registry)
    {
        if(registry->Contains<Entity>(hit))
            registry->Remove<Entity>(hit);
    }

    void LogOnCollisionEnter(Entity, Entity hit, Registry* registry)
    {
        if(auto* tag = registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Collision Enter: "} + tag->Value().data());
    }

    void LogOnCollisionExit(Entity, Entity hit, Registry* registry)
    {
        if(auto* tag = registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Collision Exit: "} + tag->Value().data());
    }

    void LogOnTriggerEnter(Entity, Entity hit, Registry* registry)
    {
        if(auto* tag = registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Trigger Enter: "} + tag->Value().data());
    }

    void LogOnTriggerExit(Entity, Entity hit, Registry* registry)
    {
        if(auto* tag = registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Trigger Exit: "} + tag->Value().data());
    }
}