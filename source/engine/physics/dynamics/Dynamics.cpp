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
void UpdatePhysicsBodies(Registry* registry, float dt)
{
    const auto g = DirectX::XMVectorScale(GravityVector, dt);
    for (auto [body, transform] : MultiView<PhysicsBody, Transform>{registry})
    {
        body->UpdateWorldInertia(transform);
        if(body->UseGravity() && !body->IsKinematic())
        {
            body->ApplyVelocity(g);
        }
    }

}

void Integrate(Registry* registry, float dt)
{
    for (auto [body, transform] : MultiView<PhysicsBody, Transform>{registry})
    {
        if(body->Integrate(transform, dt) == IntegrationResult::PutToSleep)
        {
            if constexpr(EnableSleeping)
            {
                auto* collider = registry->Get<Collider>(transform->ParentEntity());
                collider->Sleep();
            }
        }
    }
}
} // namespace nc::physics
