#pragma once

#include "math/Vector.h"

namespace nc::graphics
{
    struct Vertex
    {
        Vector3 pos;
        Vector3 normal;
        Vector2 uvs;
        Vector3 tangent;
        Vector3 bitangent;
    };
}