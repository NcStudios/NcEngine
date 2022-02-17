#pragma once

#include "ecs/Registry.h"

namespace nc::sample::prefab
{
    enum class Resource : uint32_t
    {
        Capsule,
        CapsuleBlue,
        CapsuleGreen,
        CapsuleRed,
        Cube,
        CubeBlue,
        CubeGreen,
        CubeRed,
        CubeTextured,
        Coin,
        Disc,
        DiscBlue,
        DiscGreen,
        DiscRed,
        Ground,
        RampRed,
        Sphere,
        SphereBlue,
        SphereGreen,
        SphereRed,
        PlaneGreen,
        Table,
        Token,
        Worm
    };

    std::string ToString(Resource resource);
    void InitializeResources();
    handle Create(Registry* registry, Resource resource, EntityInfo info);
} // end namespace project::prefab