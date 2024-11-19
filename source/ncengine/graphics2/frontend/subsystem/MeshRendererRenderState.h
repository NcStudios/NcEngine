#pragma once

#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
struct Batch
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
