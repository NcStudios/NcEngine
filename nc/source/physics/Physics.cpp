#include "Physics.h"
#include "PhysicsSystem.h"
#include "DebugUtils.h"

namespace nc::physics
{
    std::function<void(IClickable*)> Physics::RegisterClickable_ = nullptr;
    std::function<void(IClickable*)> Physics::UnregisterClickable_ = nullptr;
    std::function<IClickable*(LayerMask)> Physics::RaycastToClickables_ = nullptr;

    void Physics::RegisterClickable(IClickable* clickable)
    {
        IF_THROW(!Physics::RegisterClickable_, "Physics::RegisterClickable_ is not bound");
        Physics::RegisterClickable_(clickable);
    }

    void Physics::UnregisterClickable(IClickable* clickable)
    {
        IF_THROW(!Physics::UnregisterClickable_, "Physics::UnregisterClickable_ is not bound");
        Physics::UnregisterClickable_(clickable);
    }

    IClickable* Physics::RaycastToClickables(LayerMask mask)
    {
        IF_THROW(!Physics::RaycastToClickables_, "Physics::RaycastToClickables_ is not bound");
        return Physics::RaycastToClickables_(mask);
    }
}