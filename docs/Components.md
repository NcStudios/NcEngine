Components
==========

## Contents
* [Overview](#overview)
* [Registry](#registry)
* [Collider](#collider)
* [PhysicsBody](#physicsbody)

## Overview
-----------
Most user-created components should derive from AutoComponent. This provides a number of virtual functions that can be overriden to hook into the most common events.

There is also an Ecs-oriented method for defining components. Currently, it requires rebuilding the engine library.
* Create a type derived from ComponentBase that satisfies the Component concept.
* Add the type to registry_type_list (Ecs.h)
* If there is a system associated with the new type, provide a specialization of StoragePolicy enabling add/remove callbacks. These callbacks need to be set in the registry during initialization of the engine.

## Registry
-----------
All entities and components are stored in the registry. The registry keeps components pooled by type for fast iteration, and uses sparse sets for reasonably fast random lookup (it must index two arrays). This leads to certain operations potentially invalidating pointers/iterators and reordering elements.
* Adding a component may resize the internal storage, invalidating pointers/spans/etc.
The ReserveHeadroom method is available to work around this.
* Removing components uses the 'swap and pop' idiom so pointers to the last element and end iterator are invalidated.
* Newly added and destroyed entities exist in staging areas until later in the frame. Staged additions are queryable with GetEntity, but they will not appear in the active set. Staged removals are not queryable, nor do they appear in the active set, but their components are still present in their respective pools until destruction is finalized.
* Newly added components also exist in a staging area to allow adding components while iterating over pools. Calls to Contains and Get work normally while a component is staged, but Remove will only work when called in a later frame.

## Collider
-----------
When creating a Collider, the type of properties struct passed will determine the collider type. There are four options:
1. Sphere
    * Defined by a center and radius
2. Box
    * Defined by a center and extents along each axis
3. Capsule
    * Defined by a center, height, and radius
4. Hull
    * Described by an asset (.nca) file containing a vertex list for a convex hull. As this is an asset, it must be loaded before attempting to created one.

These values describing a shape cannot be modified after construction, but the shapes will be affected by the object's transform. For example, dynamically scaling a transform's scale will propagate to the collider. Transforms should always have a positive scale in each axis. Also, sphere colliders always assume uniform scaling.

There are two types of events that may result from the interaction of two colliders: collision and trigger. This determines whether to call OnCollisionXXX or OnTriggerXXX on the AutoComponents for each involved object. Additionally, collision events are sent to the physics solver. The physics solver, however, may not be allowed to modify both objects. For example, in a collision between two standard PhysicsBodies both objects may be adjusted, but if one of them is kinematic, it cannot be moved or have its velocity modified.

Properties of the Colliders and PhysicsBodies, if present, determine the event type except for one special case: no event will be detected between two static Entities. The following table outlines the result of two colliders interacting.
* Trigger:     The Collider has isTrigger == true.
* PhysicsBody: The object has a PhysicsBody.
* Kinematic:   The PhysicsBody has isKinematic == true.

|                                   | Collider | PhysicsBody | Kinematic PhysicsBody | Trigger Collider | PhysicsBody Trigger | Kinematic PhysicsBody Trigger |
| --------------------------------- | :------: | :---------: | :-------------------: | :--------------: | :-----------------: | :---------------------------: |
| **Collider**                      |   None   |  Collision  |         None          |       None       |        Trigger      |            Trigger            |
| **PhysicsBody**                   |          |  Collision  |       Collision       |      Trigger     |        Trigger      |            Trigger            |
| **Kinematic PhysicsBody**         |          |             |         None          |      Trigger     |        Trigger      |            Trigger            |
| **Trigger Collider**              |          |             |                       |       None       |        Trigger      |            Trigger            |
| **PhysicsBody Trigger**           |          |             |                       |                  |        Trigger      |            Trigger            |
| **Kinematic PhysicsBody Trigger** |          |             |                       |                  |                     |            Trigger            |

## PhysicsBody
--------------
A PhysicsBody allows an object to be manipulated by the physics solver. This enables things like gravity, physical collision response, joints, etc.

PhysicsBodies require a Collider and must be added to an Entity after the Collider.

To represent an object with infinite mass, use a mass of 0.0f. Adding a PhysicsBody to a static Entity will ignore the specified mass and use 0.0f. The center of mass is assumed to be the transform's position.

For standard PhysicsBodies, the linear and angular velocities, as well as the associated Transform, will be modified as needed by the physics system. Setting isKinematic to true still allows the physics system to build constraints involving the object, but it disallows modifications to the object's PhysicsBody and Transform.

Degrees of freedom may be restricted with the constructor's linear and angular freedom arguments. Each scalar value represents a scaling factor along the corresponding axis. All six values are 1.0f by default. To only allow rotation about the y axis, for example, the angularFreedom should be {0, 1, 0}. These values typically will be 0 or 1 (for locked or free), but intermediate values may be used, if desired.