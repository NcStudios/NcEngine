#pragma once

#include "ncengine/asset/AssetViews.h"
#include "graphics2/ShaderTypes.h"

#include "DirectXMath.h"

#include <span>

namespace nc::graphics
{
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
    BufferUpdateInfo<MeshRendererData> instanceData;
    std::vector<PassRenderState> passData;
};
} // namespace nc::graphics
