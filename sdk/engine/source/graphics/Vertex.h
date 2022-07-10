#pragma once

#include "math/Vector.h"

namespace nc
{
    struct Vertex
    {
        Vector3 Position = Vector3{0,0,0};
        Vector3 Normal = Vector3{0,0,0};
        Vector2 UV = Vector2{0,0};
        Vector3 Tangent = Vector3{0,0,0};
        Vector3 Bitangent = Vector3{0,0,0};
    };
}