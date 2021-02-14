#pragma once

#include "Ecs.h"

namespace nc::sample::prefab
{
    enum class Resource : uint32_t;

    std::string ToString(Resource resource);

    void InitializeResources();

    EntityHandle Create(Resource resource, EntityInfo info);

    //EntityHandle Create(Resource resource, Vector3 position, Quaternion rotation, Vector3 scale, std::string tag);

    // inline nc::EntityHandle Create(Resource resource, Vector3 position, Quaternion rotation, Vector3 scale)
    // {
    //     return Create(resource, position, rotation, scale, ToString(resource));
    // }

    // inline nc::EntityHandle Create(Resource resource, std::string tag)
    // {
    //     return Create(resource, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), std::move(tag));
    // }

    // inline nc::EntityHandle Create(Resource resource)
    // {
    //     return Create(resource, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), ToString(resource));
    // }

    enum class Resource : uint32_t
    {
        Beeper,
        Cube,
        CubeBlue,
        CubeGreen,
        CubeRed,
        Coin,
        Table,
        Token,
        WireframeCube,
        Worm
    };
} // end namespace project::prefab