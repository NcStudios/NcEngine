#include "MeshOptimization.h"

#include "meshoptimizer.h"

namespace nc::convert
{
auto OptimizeMesh(const std::vector<asset::MeshVertex>& verticesIn,
                  const std::vector<uint32_t>& indicesIn) -> OptimizedMesh
{
    constexpr auto vertexSize = sizeof(asset::MeshVertex);
    const auto indexCount = indicesIn.size();
    auto remap = std::vector<uint32_t>(indexCount);
    const auto vertexCount = meshopt_generateVertexRemap(
        remap.data(),
        indicesIn.data(),
        indicesIn.size(),
        verticesIn.data(),
        verticesIn.size(),
        vertexSize
    );

    auto verticesOut = std::vector<asset::MeshVertex>(vertexCount);
    auto indicesOut = std::vector<uint32_t>(indexCount);

    meshopt_remapIndexBuffer(
        indicesOut.data(),
        indicesIn.data(),
        indexCount,
        remap.data()
    );

    meshopt_remapVertexBuffer(
        verticesOut.data(),
        verticesIn.data(),
        verticesIn.size(),
        vertexSize,
        remap.data()
    );

    meshopt_optimizeVertexCache(
        indicesOut.data(),
        indicesOut.data(),
        indexCount,
        vertexCount
    );

    meshopt_optimizeOverdraw(
        indicesOut.data(),
        indicesOut.data(),
        indexCount,
        &verticesOut[0].position.x,
        vertexCount,
        vertexSize,
        1.05f
    );

    meshopt_optimizeVertexFetch(
        verticesOut.data(),
        indicesOut.data(),
        indexCount,
        verticesOut.data(),
        vertexCount,
        vertexSize
    );

    return OptimizedMesh{
        std::move(verticesOut),
        std::move(indicesOut)
    };
}
} // namespace nc::convert
