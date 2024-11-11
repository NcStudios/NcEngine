#pragma once

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
    DirectX::XMMATRIX cameraViewProjection;
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

template<typename T>
concept TriviallyCopyable = requires { requires std::is_trivially_copyable_v<T>; };

// Event data notifying changes to MaterialInstance buffer.
template<TriviallyCopyable T>
struct BufferUpdateInfo
{
    std::span<const T> instances;         // View over all instances
    std::vector<BufferSlice> dirtyRanges; // Modified subranges
};
} // namespace nc::graphics
