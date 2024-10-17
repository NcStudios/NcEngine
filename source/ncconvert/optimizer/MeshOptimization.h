#pragma once

#include "ncasset/Assets.h"

namespace nc::convert
{
struct OptimizedMesh
{
    std::vector<asset::MeshVertex> vertices;
    std::vector<uint32_t> indices;
};

auto OptimizeMesh(const std::vector<asset::MeshVertex>& vertices,
                  const std::vector<uint32_t>& indices) -> OptimizedMesh;
} // namespace nc::convert
