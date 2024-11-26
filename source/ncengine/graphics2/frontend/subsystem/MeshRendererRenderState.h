#pragma once

#include "ncengine/asset/AssetViews.h"
#include "graphics2/ShaderTypes.h"

#include "DirectXMath.h"

#include <span>

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
    explicit PassTarget(uint32_t instanceIndex, const asset::MeshView& mesh)
        : instance{instanceIndex},
          indexCount{mesh.indexCount},
          firstIndex{mesh.firstIndex},
          firstVertex{mesh.firstVertex}
    {
    }

    void UpdateMesh(const asset::MeshView& mesh)
    {
        indexCount = mesh.indexCount;
        firstIndex = mesh.firstIndex;
        firstVertex = mesh.firstVertex;
    }

    uint32_t instance;
    uint32_t indexCount;
    uint32_t firstIndex;
    uint32_t firstVertex;
};

struct PassRenderState
{
    std::vector<PassTarget> dynamicTargets;
    std::vector<PassTarget> staticTargets;
};

struct MeshRendererRenderState
{
    std::span<const MeshRendererData> instanceData;
    std::vector<PassRenderState> passData;
};
} // namespace nc::graphics
