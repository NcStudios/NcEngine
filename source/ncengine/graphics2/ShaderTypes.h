#pragma once

#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"
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
    uint32_t skyboxIndex = 0;
    uint32_t useSkybox = 0;
};

// Object model for outline pass properties used by post processing effects (type: constant buffer)
using OutlinePassData = OutlinePassProperties;

// Object model for noise pass properties used by post processing effects (type: constant buffer)
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
    uint32_t lightIndex;
    uint32_t lightFaceIndex;
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
struct MaterialData
{
    Vector4 gradientStart = Vector4::Zero();
    Vector4 gradientEnd = Vector4::One();
    Vector4 primaryColor = Vector4::One();
    Vector4 secondaryColor = Vector4::One();
    Vector4 tertiaryColor = Vector4::One();
    float normalIntensity = 1.0f;
    float hatchTiling = 1.0f;
    float gradientAmount = 0.1f;
    float reflectivity = 0.0f;
    uint32_t diffuseTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t normalTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t hatchTexIndex = std::numeric_limits<uint32_t>::max();
    uint32_t useTextureNormals = 1;
    uint32_t useFlatShading = 1;
    uint32_t useColorOverride = 0;
    uint32_t useHatchTexture = 0;
    float padding;
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
    Vector4 color = Vector4::Splat(1.0f);
    uint32_t textureIndex = std::numeric_limits<uint32_t>::max();
};

struct LightType
{
    static constexpr int Directional = 0;
    static constexpr int Point = 1;
    static constexpr int Spot = 2;
    static constexpr int Uninitialized = -1;
};

// Object model for lights (directional/point/spot) (type: StructuredBuffer element type).
struct LightData
{
    // Construct from DirectionalLight
    LightData(const Vector3& diffuseCol,
              const Vector3& specularCol,
              const float intensity_,
              const Vector3& dir,
              int32_t enableShadows,
              uint32_t viewProjIndex_)
        : diffuseColor{diffuseCol},
          type{LightType::Directional},
          specularColor{specularCol},
          direction{dir},
          intensity{intensity_},
          castsShadows{enableShadows},
          viewProjIndex{viewProjIndex_}
    {
    }

    // Construct from PointLight
    LightData(const Vector3& diffuseCol,
              const Vector3& specularCol,
              const float intensity_,
              const Vector3& pos,
              int32_t enableShadows,
              float rad,
              uint32_t viewProjIndex_)
        : diffuseColor{diffuseCol},
          type{LightType::Point},
          specularColor{specularCol},
          radius{rad},
          position{pos},
          intensity{intensity_},
          castsShadows{enableShadows},
          viewProjIndex{viewProjIndex_}
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
              uint32_t viewProjIndex_)
        : diffuseColor{diffuseCol},
          type{LightType::Spot},
          specularColor{specularCol},
          radius{rad},
          position{pos},
          innerAngle{inAngle},
          direction{dir},
          outerAngle{outAngle},
          intensity{intensity_},
          castsShadows{enableShadows},
          viewProjIndex{viewProjIndex_}
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
    uint32_t viewProjIndex = 0;
    uint32_t padding = 0;
};

// Object model for light projection matrix for shadow mapping (type: StructuredBuffer element type)
struct LightMatrixData
{
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
