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
using OnCollisionEnter_t = std::function<void(Entity self,
                                              Entity other,
                                              const HitInfo& hit,
                                              ecs::Ecs world)>;

/** @brief Callback type for collision exit events. */
using OnCollisionExit_t = std::function<void(Entity self,
                                             Entity other,
                                             ecs::Ecs world)>;

/** @brief Callback type for trigger enter events. */
using OnTriggerEnter_t = std::function<void(Entity self,
                                            Entity other,
                                            ecs::Ecs world)>;

/** @brief Callback type for trigger exit events. */
using OnTriggerExit_t = OnTriggerEnter_t;

/** @brief Component that receives collision event callbacks. */
struct CollisionListener
{
    OnCollisionEnter_t onEnter = nullptr;
    OnCollisionExit_t onExit = nullptr;
    OnTriggerEnter_t onTriggerEnter = nullptr;
    OnTriggerExit_t onTriggerExit = nullptr;
};
} // namespace nc
