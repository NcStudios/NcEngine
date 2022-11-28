#pragma once

#include "ncmath/Vector.h"

namespace nc
{
    struct Vertex
    {
        Vector3 Position = Vector3::Zero();
        Vector3 Normal = Vector3::Zero();
        Vector2 UV = Vector2::Zero();
        Vector3 Tangent = Vector3::Zero();
        Vector3 Bitangent = Vector3::Zero();
    };
}