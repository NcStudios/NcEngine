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

// Object model for MeshRenderers (type: StructuredBuffer element type).
struct MeshRendererData
{
    DirectX::XMMATRIX modelMatrix;
};

// Object model for MaterialInstance (type: StructuredBuffer element type).
struct MaterialData
{
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 gradientStart = Vector3::One();
    Vector3 gradientEnd = Vector3::One();
    Vector3 outlineColor = Vector3::One();
    float outlineWidth = 1.0f;
};

// Object model for SpotLights (type: StructuredBuffer element type).
// 128 bytes (rounded up due to alignment) with a 16-byte alignment.
struct SpotLightData
{
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::Zero();
    int castsShadows = 0;
    Vector3 color = Vector3::One();
    int isInitialized = 0;
    Vector3 direction = Vector3::One();
    float innerAngle = 1.0f;
    float outerAngle = 1.0f;
    float radius = 1.0f;
};

// Object model for PointLights (type: StructuredBuffer element type).
// 112 bytes (rounded up due to alignment) with a 16-byte alignment.
struct PointLightData
{
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::Zero();
    int castsShadows = 0;
    Vector3 color = Vector3::One();
    int isInitialized = 0;
    float radius = 1.0f;
};

// Object model for DirectionalLights (type: StructuredBuffer element type).
// Not targeting shadows for directional lights at the moment.
// 32 bytes (rounded up due to alignment) with a 16-byte alignment.
struct DirectionalLightData
{
    Vector3 color = Vector3::One();
    int isInitialized = 0;
    Vector3 direction = Vector3::One();
};

// Specifies a subrange within a buffer.
struct BufferSlice
{
    size_t offset = 0;
    size_t count = 0;
};

template<typename T>
concept TriviallyCopyable = requires { requires std::is_trivially_copyable_v<T>; };

// Event data notifying changes to StructuredBuffer.
template<TriviallyCopyable T>
struct BufferUpdateInfo
{
    std::span<const T> instances;         // View over all instances
    std::vector<BufferSlice> dirtyRanges; // Modified subranges
};
} // namespace nc::graphics
