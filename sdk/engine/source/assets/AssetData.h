#pragma once

#include "graphics/Vertex.h"

#include <span>

namespace nc
{
struct MeshBufferData
{
    std::span<const Vertex> vertices;
    std::span<const uint32_t> indices;
};
}