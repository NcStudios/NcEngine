#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"

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

    void LoadMeshes(std::vector<std::string> paths);

    struct Mesh
    {
        std::string uid;
        uint32_t firstVertex;
        uint32_t firstIndex;
        uint32_t indicesCount;
    };
}