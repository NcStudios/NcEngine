#pragma once

#include "Entity.h"
#include "math/Vector.h"

namespace nc::sample
{
    /** Options for configuring spawners.
     * 
     *  Random ranges describe an interval [-r, r], per-axis. E.g. positionRandomRange = Vector3{0, 5, 0}
     *  will spawn at y positions between -5 and 5 (plus offset, if specified).
     * 
     *  Kinematic properties describe constant velocity and rotation around an axis. If enabled, additional
     *  components will be attached to spawned objects to handle motion.
     * 
     *  Use Vector3::Zero() to disable options. The spawnAsStaticEntity property will be ignored unless all
     *  kinematic properties are zero. */

    struct SpawnBehavior
    {
        // Initial conditions
        Vector3 positionOffset = Vector3::Zero();
        Vector3 rotationOffset = Vector3::Zero();
        Vector3 positionRandomRange = Vector3::Zero();
        Vector3 rotationRandomRange = Vector3::Zero();

        // Kinematic properties
        Vector3 velocityRandomRange = Vector3::Zero();
        Vector3 rotationAxisRandomRange = Vector3::Zero();
        float thetaRandomRange = 0.0f;

        Entity::layer_type layer = 1u;
        bool spawnAsStaticEntity = false;
    };
}