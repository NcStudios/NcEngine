/**
 * @file PhysicsMaterial.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

namespace nc::physics
{
/** @brief Component with material properties used in the physics solver. */
struct PhysicsMaterial : public ComponentBase
{
    explicit PhysicsMaterial(Entity entity, float friction_ = 0.5f, float restitution_ = 0.3f) noexcept
        : ComponentBase{entity}, friction{friction_}, restitution{restitution_}
    {
    }

    float friction;
    float restitution;
};
} // namespace nc::physics
