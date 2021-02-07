#pragma once

#include "Ecs.h"

namespace nc::sample::prefab
{
    enum class Resource : uint32_t;

    void InitializeResources();

    EntityHandle Create(Resource resource, Vector3 position, Quaternion rotation, Vector3 scale, std::string tag);

    inline nc::EntityHandle Create(Resource resource, Vector3 position, Quaternion rotation, Vector3 scale)
    {
        return Create(resource, position, rotation, scale, std::string{});
    }

    inline nc::EntityHandle Create(Resource resource, std::string tag)
    {
        return Create(resource, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), std::move(tag));
    }

    inline nc::EntityHandle Create(Resource resource)
    {
        return Create(resource, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), std::string{});
    }

    enum class Resource : uint32_t
    {
        Beeper,
        Cube,
        Coin,
        Table,
        Token,
        WireframeCube,
        Worm
    };
} // end namespace project::prefab