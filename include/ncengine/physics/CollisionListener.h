/**
 * @file CollisionListener.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/physics/HitInfo.h"

#include <functional>

namespace nc
{
/** @brief Callback type for collision enter events. */
using OnCollisionHitEvent_t = std::move_only_function<void(Entity self,
                                                           Entity other,
                                                           const HitInfo& hit,
                                                           ecs::Ecs world)>;

/** @brief Callback type for collision exit, trigger enter, and trigger exit events. */
using OnCollisionEvent_t = std::move_only_function<void(Entity self,
                                                        Entity other,
                                                        ecs::Ecs world)>;

/** @brief Component that receives collision event callbacks. */
struct CollisionListener
{
    OnCollisionHitEvent_t onEnter = nullptr;
    OnCollisionEvent_t onExit = nullptr;
    OnCollisionEvent_t onTriggerEnter = nullptr;
    OnCollisionEvent_t onTriggerExit = nullptr;
};
} // namespace nc
