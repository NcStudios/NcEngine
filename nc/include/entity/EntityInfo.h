#pragma once

#include "entity/EntityHandle.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "physics/LayerMask.h"

#include <string>

namespace nc
{
    struct EntityInfo
    {
        Vector3 position = Vector3::Zero();
        Quaternion rotation = Quaternion::Identity();
        Vector3 scale = Vector3::One(); // must be nonzero
        EntityHandle parent = EntityHandle::Null();
        std::string tag = "Entity";
        physics::Layer layer = physics::DefaultLayer;
        HandleTraits::flags_type flags = 0u;
    };
}