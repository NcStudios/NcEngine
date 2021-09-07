#pragma once

#include "Ecs.h"

namespace nc::sample::prefab
{
    enum class Resource : uint32_t
    {
        Beeper,
        Capsule,
        CapsuleBlue,
        CapsuleGreen,
        CapsuleRed,
        Cube,
        CubeBlue,
        CubeGreen,
        CubeRed,
        Coin,
        Disc,
        DiscBlue,
        DiscGreen,
        DiscRed,
        Skeeball,
        Sphere,
        SphereBlue,
        SphereGreen,
        SphereRed,
        Table,
        Token,
        Worm
    };

    std::string ToString(Resource resource);
    void InitializeResources();
    Entity Create(registry_type* registry, Resource resource, EntityInfo info);
} // end namespace project::prefab