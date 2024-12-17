#include "GameLogic.h"
#include "GameLog.h"

#include "ncengine/ecs/Transform.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/RigidBody.h"

namespace nc::sample
{
void WasdBasedMovement(Entity self, ecs::Ecs world, float dt)
{
    static constexpr float speed = 2.0f;
    auto [leftRight, frontBack] = input::GetAxis() * speed * dt;
    auto q = KeyHeld(input::KeyCode::Q);
    auto e = KeyHeld(input::KeyCode::E);
    auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * speed * dt;
    world.Get<Transform>(self).Translate(Vector3{leftRight, upDown, frontBack});
}

void WasdBasedSimulatedBodyMovement(Entity self, ecs::Ecs world, float dt)
{
    static constexpr float speed = 2.0f;
    auto [leftRight, frontBack] = input::GetAxis() * speed * dt;
    auto q = KeyHeld(input::KeyCode::Q);
    auto e = KeyHeld(input::KeyCode::E);
    auto upDown = (static_cast<float>(q) - static_cast<float>(e)) * speed * dt;
    auto& transform = world.Get<Transform>(self);
    world.Get<RigidBody>(self).SetSimulatedBodyPosition(
        transform,
        transform.Position() + Vector3{leftRight, upDown, frontBack}
    );
}
} // namespace nc::sample
