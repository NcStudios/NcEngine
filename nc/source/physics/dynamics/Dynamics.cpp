#include "Dynamics.h"
#include "../PhysicsConstants.h"
#include "debug/Profiler.h"

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
        auto g_v = DirectX::XMVectorScale(DirectX::XMLoadVector3(&GravityAcceleration), dt);

        for(auto& body : bodies)
        {
            if(body.UseGravity())
            {
                body.UpdateVelocity(g_v);
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
            body.Integrate(transform, dt);
        }

        NC_PROFILE_END();
    }
}