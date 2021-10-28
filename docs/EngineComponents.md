Engine Components
=================

* [Transform](#transform)
* [Tag](#tag)
* [Collider](#collider)
* [ConcaveCollider](#concavecollider)
* [PhysicsBody](#physicsbody)
* [MeshRenderer](#meshrenderer)
* [PointLight](#pointlight)
* [AudioSource](#audiosource)
* [Camera](#camera)
* [ParticleEmitter](#particleemitter)
* [NetworkDispatcher](#networkdispatcher)

### Transform
------------
A transform represents the transformation matrix of an entity. Unlike most components, transforms do not need to be manually added as the registry does this when creating an entity. A transform's constructor arguments are taken from the EntityInfo struct used when creating an entity.

Any quaternions supplied to a transform should be unit quaternions, and scale values should be nonzero.

Transforms can be nested to form entity hierarchies using the SetParent member function or in the 'parent' member of the EntityInfo struct. Transforms have two matrices to represent their world and local space values. Values from either matrix can be queried, while only local space values may be directly modified. Changes to a local space matrix will automatically propagate to the affected world space matrices.

### Tag
-------
A Tag holds a string identifier for an Entity. They are primarily used in the editor scene graph to provide more context. Like Transforms, they are automatically added. The default tag is "Entity".

### Collider
------------
When creating a Collider, the type of properties struct passed will determine the collider type. There are four options:
1. Sphere
    * Defined by a center and radius
2. Box
    * Defined by a center and extents along each axis
3. Capsule
    * Defined by a center, height, and radius
4. Hull
    * Described by an [asset](Overview.md#assets) file containing a vertex list for a convex hull. As this is an asset, it must be loaded before attempting to create one.

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

### ConcaveCollider
------------------
Concave colliders exist to support collision detection against objects that can't be accurately modelled with a standard collider. They are less efficient, so prefer approximating volumes with standard colliders over concave colliders. To minimize their added cost, concave colliders may only be added to static entities and do not collide with one another. Constructing a concave collider requires a path to an [asset](Overview.md#assets) file specifying the geometry as a triangle list.

Concave colliders act more like a collision surface than a volume. They may be used to represent unenclosed objects (imagine a deformed plane representing a topography). When used to represent an enclosed volume, they have no concept of their interior or exterior - just the surface. This makes them unsuitable for use as triggers.

### PhysicsBody
--------------
A PhysicsBody allows an object to be manipulated by the physics solver. This enables things like gravity, physical collision response, joints, etc.

PhysicsBodies require a Collider and must be added to an Entity after the Collider.

To represent an object with infinite mass, use a mass of 0.0f. Adding a PhysicsBody to a static Entity will ignore the specified mass and use 0.0f. The center of mass is assumed to be the transform's position.

For standard PhysicsBodies, the linear and angular velocities, as well as the associated Transform, will be modified as needed by the physics system. Setting isKinematic to true still allows the physics system to build constraints involving the object, but it disallows modifications to the object's PhysicsBody and Transform.

Degrees of freedom may be restricted with the constructor's linear and angular freedom arguments. Each scalar value represents a scaling factor along the corresponding axis. All six values are 1.0f by default. To only allow rotation about the y axis, for example, the angularFreedom should be {0, 1, 0}. These values typically will be 0 or 1 (for locked or free), but intermediate values may be used, if desired.

### MeshRenderer
---------------
Mesh renderers allows a mesh to be rendered using a material. They rely on multiple [assets](Overview.md#assets): one the mesh and four for the material's textures. They also require a technique type, but only one type is currently exposed in the API. Rendering will use the associated entity's transform for vertex transformations.

### PointLight
-------------
Point lights components specify a light source at a point in space emitting light in all directions. The light's position is the position of the associated transform and will be updated if the transform moves.

### AudioSource
--------------
Audio sources are used to play sounds in a scene. Construction requires a path to an audio clip [asset](Overview.md#assets). Audio sources may optionally be spatial, in which case the associated transform's position will be used as the sound source location (3D sound is a work in progress). 

An audio listener must be registered for audio sources to play (Audio.h), even if spatial sound isn't used.

### Camera
---------
Camera properties have not yet been moved into the camera component. Some can be set in the config file, while others are still internally managed. The only current use for the camera component is for calling SetMainCamera (MainCamera.h).

### ParticleEmitter
------------------
Particle emitters have not yet been updated from DirectX to Vulkan.

### NetworkDispatcher
---------------------