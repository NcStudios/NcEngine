#include "Dynamics.h"
#include "../PhysicsConstants.h"
#include "debug/Profiler.h"

namespace
{
    const auto GravityVector = DirectX::XMVectorSet(0.0f, nc::physics::Gravity, 0.0f, 0.0f);
}

namespace nc::physics
{
    void UpdateWorldInertiaTensors(registry_type* registry, std::span<PhysicsBody> bodies)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        for(auto& body : bodies)
        {
            auto* transform = registry->Get<Transform>(body.GetParentEntity());
            /** @todo Can this be skipped for static bodies? */
            body.UpdateWorldInertia(transform);
        }

        NC_PROFILE_END();
    }

    void ApplyGravity(std::span<PhysicsBody> bodies, float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);
        auto g = DirectX::XMVectorScale(GravityVector, dt);

        for(auto& body : bodies)
        {
            if(body.UseGravity())
            {
                body.ApplyVelocity(g);
            }
        }

        NC_PROFILE_END();
    }

    void Integrate(registry_type* registry, std::span<PhysicsBody> bodies, float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        for(auto& body : bodies)
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

        NC_PROFILE_END();
    }
}