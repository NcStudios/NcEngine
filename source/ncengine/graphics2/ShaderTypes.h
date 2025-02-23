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
    DirectX::XMMATRIX cameraInvProjection = DirectX::XMMatrixIdentity();
    Vector3 cameraPosition = Vector3::One();
    uint32_t lightCount = 0;
    float nearClip = 0.1f;
    float farClip = 400.0f;
    int padding1 = 0;
    int padding2 = 0;
};

// Object model for outline pass properties used by post processing effects (type: constant buffer)
struct OutlinePassData
{
    Vector3 color = Vector3::Zero();
    float width = 1.0f;
    float depthThreshold = 0.8f;
    float viewDirDepthThreshold = 0.4f;
    float normalThreshold = 0.4f;
};

// Object model for outline pass properties used by post processing effects (type: constant buffer)
struct NoisePassData
{
    Vector3 maskGradientStart = Vector3::Zero();
    float maskGradientAmount = 0.1f;
    Vector3 maskGradientEnd = Vector3::One();
    uint32_t noiseTexIndex = 0u;
    float noiseTexAmount = 0.1f;
    float noiseTexTiling = 1.0f;
};

// Object model for specifying the index into the color and depth offscreen render target arrays. Limited to four of each type of index
struct PostProcessSinkIndexData
{
    int32_t colorRenderTargetIndex1;
    int32_t colorRenderTargetIndex2;
    int32_t colorRenderTargetIndex3;
    int32_t colorRenderTargetIndex4;
    int32_t depthRenderTargetIndex1;
    int32_t depthRenderTargetIndex2;
    int32_t depthRenderTargetIndex3;
    uint32_t hasPostProcessTarget;
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
    DirectX::XMMATRIX invModelMatrix = DirectX::XMMatrixIdentity();
};

// Object model for MaterialInstance (type: StructuredBuffer element type).
struct MaterialData
{
    Vector3 gradientStart = Vector3::Zero();
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    Vector3 gradientEnd = Vector3::One();
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t hatchTexIndex = std::numeric_limits<uint32_t>::max();
    float normalIntensity = 1.0f;
    float hatchTiling = 1.0f;
    float gradientAmount = 0.1f;
    float reflectivity = 0.0f;
    uint32_t useTextureNormals = 0;
    float padding0 = 0.0f;
    float padding1 = 0.0f;
};

// Object model for animated bones (type: StructuredBuffer element type).
struct BoneData
{
    DirectX::XMMATRIX animatedBoneMatrix = DirectX::XMMATRIX{};
};

// Object model for particles (type: StructuredBuffer element type).
struct ParticleData
{
    DirectX::XMMATRIX model = DirectX::XMMATRIX{};
    uint32_t textureIndex = std::numeric_limits<uint32_t>::max();
};

// Object model for lights (directional/point/spot) (type: StructuredBuffer element type).
struct LightData
{
    struct LightType
    {
        static constexpr int Directional = 0;
        static constexpr int Point = 1;
        static constexpr int Spot = 2;
        static constexpr int Uninitialized = -1;
    };

    // Construct from DirectionalLight
    LightData(const Vector3& diffuseCol,
              const Vector3& specularCol,
              const float intensity_,
              const Vector3& dir)
        : diffuseColor{diffuseCol},
          specularColor{specularCol},
          intensity{intensity_},
          type{LightType::Directional},
          direction{dir}
    {
    }

    // Construct from PointLight
    LightData(const Vector3& diffuseCol,
              const Vector3& specularCol,
              const float intensity_,
              const Vector3& pos,
              int32_t enableShadows,
              float rad,
              DirectX::FXMMATRIX viewProj)
        : diffuseColor{diffuseCol},
          specularColor{specularCol},
          intensity{intensity_},
          type{LightType::Point},
          position{pos},
          radius{rad},
          castsShadows{enableShadows},
          viewProjection{viewProj}
    {
    }

    // Construct from SpotLight
    LightData(const Vector3& diffuseCol,
              const Vector3& specularCol,
              const float intensity_,
              const Vector3& pos,
              float inAngle,
              const Vector3& dir,
              float outAngle,
              float rad,
              int32_t enableShadows,
              DirectX::FXMMATRIX viewProj)
        : diffuseColor{diffuseCol},
          specularColor{specularCol},
          intensity{intensity_},
          type{LightType::Spot},
          position{pos},
          innerAngle{inAngle},
          direction{dir},
          outerAngle{outAngle},
          radius{rad},
          castsShadows{enableShadows},
          viewProjection{viewProj}
    {
    }

    Vector3 diffuseColor = Vector3::One();
    int type = LightType::Uninitialized;
    Vector3 specularColor = Vector3::One();
    float radius = 1.0f;
    Vector3 position = Vector3::Zero();
    float innerAngle = 1.0f;
    Vector3 direction = Vector3::Down();
    float outerAngle = 1.0f;
    float intensity = 1.0f;
    int castsShadows = 0;
    DirectX::XMMATRIX viewProjection = DirectX::XMMATRIX{};
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
