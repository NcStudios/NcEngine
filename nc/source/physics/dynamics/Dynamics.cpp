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

        auto g = GravityAcceleration * dt;

        for(auto& body : bodies)
        {
            auto& properties = body.GetProperties();

            if(properties.useGravity)
            {
                properties.velocity += g;
            }
        }

        NC_PROFILE_END();
    }

    void Integrate(registry_type* registry, std::span<PhysicsBody> bodies, float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        for(auto& body : bodies)
        {
            Entity entity = body.GetParentEntity();

            if(EntityUtils::IsStatic(entity))
                continue;
            
            auto& properties = body.GetProperties();
            auto* transform = registry->Get<Transform>(entity);

            properties.velocity = HadamardProduct(properties.linearFreedom, properties.velocity);
            properties.angularVelocity = HadamardProduct(properties.angularFreedom, properties.angularVelocity);

            transform->Translate(properties.velocity * dt);
            transform->Rotate(Quaternion::FromEulerAngles(properties.angularVelocity * dt));

            properties.velocity *= pow(1.0f - properties.drag, dt);
            properties.angularVelocity *= pow(1.0f - properties.angularDrag, dt);
        }

        NC_PROFILE_END();
    }
}