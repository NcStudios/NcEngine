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
// 96 bytes with a 16-byte alignment.
struct GlobalEnvironmentData
{
    DirectX::XMMATRIX cameraViewProjection = DirectX::XMMatrixIdentity();
    Vector3 cameraPosition = Vector3::One();
    uint32_t lightCount = 0;
    // uint32_t dirLightsCount = 0;
    // uint32_t pointLightsCount = 0;
    // uint32_t spotLightsCount = 0;
    // Vector2 padding = Vector2::Zero();
};

// Object model for outline pass properties used by post processing effects (type: constant buffer)
// 16 bytes with 4 byte alignment
struct OutlinePassData
{
    Vector3 color = Vector3::Zero();
    float width = 1.0f;
};

// Object model for specifying the index into the color and depth offscreen render target arrays. Limited to four of each type of index
// 32 bytes with 4 byte alignment
struct PostProcessSinkIndexData
{
    uint32_t colorRenderTargetIndex1;
    uint32_t colorRenderTargetIndex2;
    uint32_t colorRenderTargetIndex3;
    uint32_t colorRenderTargetIndex4;
    uint32_t depthRenderTargetIndex1;
    uint32_t depthRenderTargetIndex2;
    uint32_t depthRenderTargetIndex3;
    uint32_t depthRenderTargetIndex4;
};

// Object model for StaticMeshes (type: StructuredBuffer element type).
struct StaticMeshInstanceData
{
    uint32_t transformIndex = std::numeric_limits<uint32_t>::max();
    uint32_t materialIndex = std::numeric_limits<uint32_t>::max();
};

// Object model for SkinnedMeshes (type: StructuredBuffer element type).
struct SkinnedMeshInstanceData
{
    uint32_t transformIndex = std::numeric_limits<uint32_t>::max();
    uint32_t materialIndex = std::numeric_limits<uint32_t>::max();
    uint32_t boneIndex = std::numeric_limits<uint32_t>::max();
};

// Object model for Transforms (type: StructuredBuffer element type).
struct TransformData
{
    DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixIdentity();
};

// Object model for MaterialInstance (type: StructuredBuffer element type).
// 48 bytes with a 16-byte alignment.
struct MaterialData
{
    Vector3 gradientStart = Vector3::Splat(10.0f);
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 gradientEnd = Vector3::Splat(11.0f);
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 outlineColor = Vector3::Splat(12.0f);
    float outlineWidth = 1.0f;
};

// Object model for animated bones (type: StructuredBuffer element type).
struct BoneData
{
    DirectX::XMMATRIX animatedBoneMatrix = DirectX::XMMATRIX{};
};


struct LightType
{
    static constexpr auto Directional = 0;
    static constexpr auto Point = 1;
    static constexpr auto Spot = 2;
    static constexpr auto Uninitialized = -1;
};

struct LightData
{

    // todo: fix/reorder c'tors
    LightData(const Vector3& col,
              const Vector3& dir)
        : color{col},
          type{LightType::Directional},
          direction{dir}
    {
    }

    LightData(const Vector3& col,
              const Vector3& pos,
              int32_t castsShadows_,
              float rad,
              DirectX::FXMMATRIX viewProj)
        : color{col},
          type{LightType::Point},
          position{pos},
          radius{rad},
          castsShadows{castsShadows_},
          viewProjection{viewProj}
    {
    }

    LightData(const Vector3& col,
              const Vector3& pos,
              float innerAngle_,
              const Vector3& dir,
              float outerAngle_,
              float rad,
              int32_t castsShadows_,
              DirectX::FXMMATRIX viewProj)
        : color{col},
          type{LightType::Spot},
          position{pos},
          innerAngle{innerAngle_},
          direction{dir},
          outerAngle{outerAngle_},
          radius{rad},
          castsShadows{castsShadows_},
          viewProjection{viewProj}
    {
    }

    Vector3 color = Vector3::One();
    int type = LightType::Uninitialized; // 0: Directional, 1: Point, 2: Spot

    Vector3 position = Vector3::Zero(); // Only used for Point and Spot lights
    float innerAngle = 1.0f;  // Only used for Spot lights

    Vector3 direction = Vector3::Down();
    float outerAngle = 1.0f;  // Only used for Spot lights

    float radius = 1.0f;      // Only used for Point and Spot lights
    int castsShadows = 0;
    int pad1; // combine?
    int pad2;

    DirectX::XMMATRIX viewProjection = DirectX::XMMATRIX{}; // Shadow mapping
};


// Object model for DirectionalLights (type: StructuredBuffer element type).
// Not targeting shadows for directional lights at the moment.
// 32 bytes with a 16-byte alignment.
struct DirectionalLightData
{ 
    DirectionalLightData(Vector3 color_, Vector3 direction_)
        : color{color_},
          direction{direction_}{}

    Vector3 color = Vector3::One();
    float padding1 = 0.0f;
    Vector3 direction = Vector3::One();
    float padding2 = 0.0f;
};

// Object model for PointLights (type: StructuredBuffer element type).
// 96 bytes with a 16-byte alignment.
struct PointLightData
{
    PointLightData(DirectX::XMMATRIX viewProjection_,
                   Vector3 position_,
                   int32_t castsShadows_,
                   Vector3 color_,
                   float radius_)
        : viewProjection{viewProjection_},
          position{position_},
          castsShadows{castsShadows_},
          color{color_},
          radius{radius_}{}
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::Zero();
    int castsShadows = 0;
    Vector3 color = Vector3::One();
    float radius = 1.0f;
};


// Object model for SpotLights (type: StructuredBuffer element type).
// 128 bytes with a 16-byte alignment.
struct SpotLightData
{
    SpotLightData(DirectX::XMMATRIX viewProjection_,
                  Vector3 position_,
                  int32_t castsShadows_,
                  Vector3 color_,
                  float innerAngle_,
                  Vector3 direction_,
                  float outerAngle_,
                  float radius_)
        : viewProjection{viewProjection_},
          position{position_},
          castsShadows{castsShadows_},
          color{color_},
          innerAngle{innerAngle_},
          direction{direction_},
          outerAngle{outerAngle_},
          radius{radius_}{}
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::Zero();
    int castsShadows = 0;
    Vector3 color = Vector3::One();
    float innerAngle = 1.0f;
    Vector3 direction = Vector3::One();
    float outerAngle = 1.0f;
    Vector3 padding = Vector3::Zero();
    float radius = 1.0f;
};

// Object model for WireframeRenderers (type: constant buffer)
struct WireframeData
{
    DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixIdentity();
    Vector4 color = Vector4::One();
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
