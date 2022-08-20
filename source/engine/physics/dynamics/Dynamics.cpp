#include "Dynamics.h"
#include "ecs/View.h"
#include "physics/Collider.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsConstants.h"

namespace
{
const auto GravityVector = DirectX::XMVectorSet(0.0f, nc::physics::Gravity, 0.0f, 0.0f);
}

namespace nc::physics
{
void UpdateWorldInertiaTensors(Registry* registry)
{
    for(auto& body : View<PhysicsBody>{registry})
    {
        const auto* transform = registry->Get<Transform>(body.ParentEntity());
        body.UpdateWorldInertia(transform);
    }
}

void ApplyGravity(Registry* registry, float dt)
{
    const auto g = DirectX::XMVectorScale(GravityVector, dt);

    for(auto& body : View<PhysicsBody>{registry})
    {
        if(body.UseGravity())
        {
            body.ApplyVelocity(g);
        }
    }
}

void Integrate(Registry* registry, float dt)
{
    for(auto& body : View<PhysicsBody>{registry})
    {
        auto* transform = registry->Get<Transform>(body.ParentEntity());

        if(body.Integrate(transform, dt) == IntegrationResult::PutToSleep)
        {
            if constexpr(EnableSleeping)
            {
                auto* collider = registry->Get<Collider>(body.ParentEntity());
                collider->Sleep();
            }
        }
    }
}
} // namespace nc::physics
