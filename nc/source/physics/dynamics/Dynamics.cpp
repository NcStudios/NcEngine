#include "Dynamics.h"
#include "ecs/component/Collider.h"
#include "ecs/component/PhysicsBody.h"
#include "physics/PhysicsConstants.h"
#include "debug/Profiler.h"

namespace
{
    const auto GravityVector = DirectX::XMVectorSet(0.0f, nc::physics::Gravity, 0.0f, 0.0f);
}

namespace nc::physics
{
    void UpdateWorldInertiaTensors(Registry* registry)
    {
        for(auto& body : registry->ViewAll<PhysicsBody>())
        {
            const auto* transform = registry->Get<Transform>(body.GetParentEntity());
            body.UpdateWorldInertia(transform);
        }
    }

    void ApplyGravity(Registry* registry, float dt)
    {
        const auto g = DirectX::XMVectorScale(GravityVector, dt);

        for(auto& body : registry->ViewAll<PhysicsBody>())
        {
            if(body.UseGravity())
            {
                body.ApplyVelocity(g);
            }
        }
    }

    void Integrate(Registry* registry, float dt)
    {
        for(auto& body : registry->ViewAll<PhysicsBody>())
        {
            auto* transform = registry->Get<Transform>(body.GetParentEntity());

            if(body.Integrate(transform, dt) == IntegrationResult::PutToSleep)
            {
                if constexpr(EnableSleeping)
                {
                    auto* collider = registry->Get<Collider>(body.GetParentEntity());
                    collider->Sleep();
                }
            }
        }
    }
}