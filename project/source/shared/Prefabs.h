#pragma once

#include "ecs/Entity.h"

namespace nc { class Registry; }

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
        Coin,
        Disc,
        DiscBlue,
        DiscGreen,
        DiscRed,
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
    Entity Create(Registry* registry, Resource resource, EntityInfo info);
} // end namespace project::prefab