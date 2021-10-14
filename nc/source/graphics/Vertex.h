#pragma once

#include "math/Vector.h"

namespace nc::graphics
{
    struct Vertex
    {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
        Vector3 Tangent;
        Vector3 Bitangent;
    };
}