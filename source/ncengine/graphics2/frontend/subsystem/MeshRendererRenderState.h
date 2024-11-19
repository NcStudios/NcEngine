#pragma once

#include "ncengine/asset/AssetViews.h"
#include "graphics2/ShaderTypes.h"

#include "DirectXMath.h"

#include <span>

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

// struct PassTarget
// {
//     explicit PassTarget(uint32_t instanceIndex, const asset::MeshView& mesh)
//         : instance{instanceIndex},
//           indexCount{mesh.indexCount},
//           firstIndex{mesh.firstIndex},
//           firstVertex{mesh.firstVertex}
//     {
//     }

//     void UpdateMesh(const asset::MeshView& mesh)
//     {
//         indexCount = mesh.indexCount;
//         firstIndex = mesh.firstIndex;
//         firstVertex = mesh.firstVertex;
//     }

//     uint32_t instance;
//     uint32_t indexCount;
//     uint32_t firstIndex;
//     uint32_t firstVertex;
// };

// struct PassRenderState
// {
//     std::vector<PassTarget> targets;
// };

struct MeshRendererRenderState
{
    BufferUpdateInfo<TransformData> transformData;
    BufferUpdateInfo<InstanceData> instanceData;
    std::vector<std::vector<Batch>> passBatches;
};
} // namespace nc::graphics
