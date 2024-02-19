#include "GameLogic.h"
#include "GameLog.h"

#include "ncengine/ecs/Transform.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc::sample
{
void WasdBasedMovement(Entity self, Registry* registry, float dt)
{
    static constexpr float speed = 2.0f;
    auto [leftRight, frontBack] = input::GetAxis() * speed * dt;
    auto q = KeyHeld(input::KeyCode::Q);
    auto e = KeyHeld(input::KeyCode::E);
    auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * speed * dt;
    registry->Get<Transform>(self)->Translate(Vector3{leftRight, upDown, frontBack});
}

void DestroyOnTriggerExit(Entity, Entity hit, Registry* registry)
{
    if(registry->Contains<Entity>(hit))
        registry->Remove<Entity>(hit);
}

void LogOnCollisionEnter(Entity, Entity hit, Registry* registry)
{
    if(registry->Contains<Entity>(hit))
    {
        auto* tag = registry->Get<Tag>(hit);
        GameLog::Log(std::string{"Collision Enter: "} + tag->value.c_str());
    }
}

void LogOnCollisionExit(Entity, Entity hit, Registry* registry)
{
    if(registry->Contains<Entity>(hit))
    {
        auto* tag = registry->Get<Tag>(hit);
        GameLog::Log(std::string{"Collision Exit: "} + tag->value.c_str());
    }
}

void LogOnTriggerEnter(Entity, Entity hit, Registry* registry)
{
    if(registry->Contains<Entity>(hit))
    {
        auto* tag = registry->Get<Tag>(hit);
        GameLog::Log(std::string{"Trigger Enter: "} + tag->value.c_str());
    }
}

void LogOnTriggerExit(Entity, Entity hit, Registry* registry)
{
    if(registry->Contains<Entity>(hit))
    {
        auto* tag = registry->Get<Tag>(hit);
        GameLog::Log(std::string{"Trigger Exit: "} + tag->value.c_str());
    }
}
}
