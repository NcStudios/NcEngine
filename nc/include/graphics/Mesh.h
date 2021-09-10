#pragma once

#include "math/Vector.h"

#include <string>
#include <vector>

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

    struct Mesh
    {
        uint32_t firstVertex;
        uint32_t firstIndex;
        uint32_t indicesCount;
    };

    void LoadMeshes(const std::vector<std::string>& paths);
}