#include "Physics.h"
#include "PhysicsSystem.h"

namespace nc::physics
{
    std::function<void(IClickable*)> Physics::RegisterClickable_ = nullptr;
    std::function<void(IClickable*)> Physics::UnregisterClickable_ = nullptr;
    std::function<IClickable*(LayerMask)> Physics::RaycastToClickables_ = nullptr;

    void Physics::RegisterClickable(IClickable* clickable)
    {
        Physics::RegisterClickable_(clickable);
    }

    void Physics::UnregisterClickable(IClickable* clickable)
    {
        Physics::UnregisterClickable_(clickable);
    }

    IClickable* Physics::RaycastToClickables(LayerMask mask)
    {
        return Physics::RaycastToClickables_(mask);
    }
}