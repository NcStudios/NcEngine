#pragma once

#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
struct Batch
{
    explicit Batch(uint32_t instanceIndex, const asset::MeshView& mesh)
        : firstInstance{instanceIndex},
          instanceCount{0},
          firstIndex{mesh.firstIndex},
          indexCount{mesh.indexCount},
          vertexOffset{mesh.firstVertex}
    {
    }

    uint32_t firstInstance;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t vertexOffset;
};

struct MeshRenderState
{
    BufferUpdateInfo<TransformData> transformData;
    BufferUpdateInfo<StaticMeshInstanceData> staticMeshInstanceData;
    std::vector<std::vector<Batch>> staticMeshBatches;
};
} // namespace nc::graphics
