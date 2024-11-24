#pragma once

#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
// @todo 808 Replace PassTarget with this
struct Batch
{
    explicit Batch(uint32_t instanceIndex, const asset::MeshView& mesh)
        : instanceOffset{instanceIndex},
          instanceCount{0},
          indexOffset{mesh.firstIndex},
          indexCount{mesh.indexCount},
          vertexOffset{mesh.firstVertex}
    {
    }

    uint32_t instanceOffset;
    uint32_t instanceCount;
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;
};

struct PassTarget
{
    uint32_t instanceOffset = UINT32_MAX;
    uint32_t instanceCount = UINT32_MAX;
    uint32_t indexOffset = UINT32_MAX;
    uint32_t indexCount = UINT32_MAX;
    uint32_t vertexOffset = UINT32_MAX;
};

struct MeshRendererRenderState
{
    BufferUpdateInfo<TransformData> transformData;
    BufferUpdateInfo<InstanceData> instanceData;
    std::vector<std::vector<Batch>> passBatches;
};
} // namespace nc::graphics
