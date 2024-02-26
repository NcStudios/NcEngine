/**
 * @file PhysicsMaterial.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

namespace nc::physics
{
/**
 * @brief Component with material properties used by the physics solver.
 * 
 * During collision resolution, PhysicsMaterial properties of interacting objects are combined
 * multiplicatively. Objects without a PhysicsMaterial are assigned default values. An Entity
 * does not need a PhysicsBody in order to have a PhysicsMaterial.
*/
struct PhysicsMaterial
{
    float friction = 0.5f;    // [0, 1]
    float restitution = 0.3f; // [0, 1]
};
} // namespace nc::physics
