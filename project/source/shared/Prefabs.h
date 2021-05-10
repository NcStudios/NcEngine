#pragma once

#include "Ecs.h"

namespace nc::sample::prefab
{
    enum class Resource : uint32_t
    {
        Beeper,
        Cube,
        CubeBlue,
        CubeGreen,
        CubeRed,
        Coin,
        Sphere,
        Table,
        Token,
        WireframeCube,
        Worm,
        Sun,
        Mercury,
        Venus,
        Earth,
        Mars,
        Jupiter,
        Saturn,
        Uranus,
        Neptune,
        Pluto
    };

    std::string ToString(Resource resource);
    void InitializeResources();
    EntityHandle Create(Resource resource, EntityInfo info);
} // end namespace project::prefab