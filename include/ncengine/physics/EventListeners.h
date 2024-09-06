/**
 * @file EventListener.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "HitInfo.h"
#include "ncengine/ecs/EcsFwd.h"

#include <functional>

namespace nc::physics
{
/** @brief Callback type for collision enter events. */
using OnCollisionEnter_t = std::function<void(Entity self,
                                              Entity other,
                                              const HitInfo& hit,
                                              ecs::Ecs world)>;

/** @brief Callback type for collision exit events. */
using OnCollisionExit_t = std::function<void(Entity self,
                                             Entity other,
                                             ecs::Ecs world)>;

/** @brief Component that receives collision event callbacks. */
struct CollisionListener
{
    OnCollisionEnter_t onEnter = nullptr;
    OnCollisionExit_t onExit = nullptr;
};
} // namespace nc::physics
