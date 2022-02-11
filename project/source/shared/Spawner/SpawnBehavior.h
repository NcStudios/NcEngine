#pragma once

#include "ecs/Entity.h"
#include "math/Vector.h"

namespace nc::sample
{
    /** Options for configuring spawners. Use Vector3::Zero() to disable kinematic
     *  options. If enabled, additional components will be attached. */
    struct SpawnBehavior
    {
        // Initial conditions
        Vector3 minPosition = Vector3::Zero();
        Vector3 maxPosition = Vector3::Zero();
        Vector3 minRotation = Vector3::Zero();
        Vector3 maxRotation = Vector3::Zero();

        // Kinematic properties
        Vector3 minVelocity = Vector3::Zero();
        Vector3 maxVelocity = Vector3::Zero();
        Vector3 rotationAxis = Vector3::Zero();
        float rotationTheta = 0.0f;

        // Object properties
        Entity::layer_type layer = 1u;
        Entity::flags_type flags = Entity::Flags::None;
    };
}