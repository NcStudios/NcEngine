#pragma once

#include "Ecs.h"

namespace nc::physics
{
    /** Specifies how a Collider interacts with other colliders. */
    enum class ColliderInteractionType
    {
        Collider,               // Default Collider
        Physics,                // Collider and PhysicsBody
        KinematicPhysics,       // Collider and PhysicsBody with isKinematic == true
        ColliderTrigger,        // Collider with isTrigger == true
        PhysicsTrigger,         // Collider with isTrigger == true and PhysicsBody
        KinematicPhysicsTrigger // Collider and PhysicsBody with both flags == true
    };

    /** Specifies how to respond to a collision event. A physics response may be
     *  restricted to a single body but will always notify both entities. */
    enum class CollisionEventType
    {
        None, Trigger, TwoBodyPhysics, FirstBodyPhysics, SecondBodyPhysics
    };

    struct NormalData
    {
        Vector3 normal;
        float distance;
    };

    struct Contact
    {
        Vector3 worldPointA;
        Vector3 worldPointB;
        Vector3 localPointA;
        Vector3 localPointB;
        Vector3 normal;
        float depth;
        float lambda;
        float muTangent;
        float muBitangent;
        bool success;
    };

    /** @todo Manifold could use array for contact points */

    struct Manifold
    {
        Entity entityA;
        Entity entityB;
        CollisionEventType eventType;
        std::vector<Contact> contacts;

        int AddContact(const Contact& contact);
        int SortPoints(const Contact& contact);
        void UpdateWorldPoints(registry_type* registry);
        const Contact& GetDeepestContact() const;
    };
}