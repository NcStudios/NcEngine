/**
 * @file PhysicsAnimator.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/ecs/Entity.h"
#include "DirectXMath.h"

namespace nc
{
/** @brief Interface for hooking into NcPhysics animation and Transform synchronization. */
class PhysicsAnimator
{
    public:
        virtual ~PhysicsAnimator() = default;

        virtual void Animate(nc::Entity entity,
                             DirectX::FXMVECTOR position,
                             DirectX::FXMVECTOR rotation) = 0;
};
} // namespace nc
