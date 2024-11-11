#pragma once

#include "ncengine/graphics/Material.h"
#include "ncmath/Vector.h"

#include "DirectXMath.h"

#include <limits>
#include <span>
#include <vector>

namespace nc::graphics
{
// Object model for environment data (type: constant buffer)
struct GlobalEnvironmentData
{
    DirectX::XMMATRIX cameraViewProjection = DirectX::XMMatrixIdentity();
};

// Object model for MeshRenderers (type: StructuredBuffer element type).
struct MeshRendererData
{
    DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixIdentity();
    uint32_t materialIndex = NullMaterialInstanceHandle;
    // uint32_t padding[3];
    // todo: padding ??
};

// Object model for MaterialInstance (type: StructureBuffer element type).
struct MaterialData
{
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 gradientStart = Vector3::One();
    Vector3 gradientEnd = Vector3::One();
    Vector3 outlineColor = Vector3::One();
    float outlineWidth = 1.0f;
};

// Specifies a subrange within a buffer.
struct BufferSlice
{
    size_t offset = 0;
    size_t count = 0;
};

// Event data notifying changes to MaterialInstance buffer.
struct MaterialDataUpdateInfo
{
    std::span<const MaterialData> instances; // view over all instances
    std::vector<BufferSlice> dirtyRanges;    // modified subranges
};
} // namespace nc::graphics
