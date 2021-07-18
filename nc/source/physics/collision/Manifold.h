#pragma once

#include "Ecs.h"

namespace nc::physics
{
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
        bool success;
    };

    struct Manifold
    {
        Entity entityA;
        Entity entityB;
        std::vector<Contact> contacts;

        int AddContact(const Contact& contact);
        int SortPoints(const Contact& contact);
        void UpdateWorldPoints(registry_type* registry);
        const Contact& GetDeepestContact() const;
    };
}