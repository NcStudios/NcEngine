#include "NcPhysics.h"
#include "physics/PhysicsSystem.h"

namespace nc::physics
{
    void NcRegisterClickable(IClickable* clickable)
    {
        PhysicsSystem::RegisterClickable(clickable);
    }

    void NcUnregisterClickable(IClickable* clickable)
    {
        PhysicsSystem::UnregisterClickable(clickable);
    }

    IClickable* NcRaycastToClickables(LayerMask mask)
    {
        return PhysicsSystem::RaycastToClickables(mask);
    }
}