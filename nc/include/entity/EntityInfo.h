#pragma once

#include "math/Vector3.h"
#include "math/Quaternion.h"

#include <string>

namespace nc
{
    struct EntityInfo
    {
        Vector3 position = Vector3::Zero();
        Quaternion rotation = Quaternion::Identity();
        Vector3 scale = Vector3::One(); // must be nonzero
        std::string tag = "Entity";
        bool isStatic = false;
    };
}