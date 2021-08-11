#pragma once

#include "math/Vector.h"

#include <string>
#include <vector>

namespace nc::graphics::vulkan
{
    struct Vertex
    {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
        Vector3 Tangent;
        Vector3 Bitangent;
    };

    void LoadMeshes(const std::vector<std::string>& paths);

    struct Mesh
    {
        std::string uid;
        uint32_t firstVertex;
        uint32_t firstIndex;
        uint32_t indicesCount;
    };
}