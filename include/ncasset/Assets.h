/**
 * @file Assets.h.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Geometry.h"
#include "DirectXMath.h"

#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace nc::asset
{
struct AudioClip
{
    size_t samplesPerChannel;
    std::vector<double> leftChannel;
    std::vector<double> rightChannel;
};

struct VertexSpaceToBoneSpace
{
    std::string boneName;
    DirectX::XMMATRIX transformationMatrix;
};

struct BoneSpaceToParentSpace
{
    std::string boneName;
    DirectX::XMMATRIX transformationMatrix;
    uint32_t numChildren;
    uint32_t indexOfFirstChild;
};

struct BonesData
{
    std::unordered_map<std::string, uint32_t> boneMapping;
    std::vector<VertexSpaceToBoneSpace> vertexSpaceToBoneSpace;
    std::vector<BoneSpaceToParentSpace> boneSpaceToParentSpace;
};

struct HullCollider
{
    Vector3 extents;
    float maxExtent;
    std::vector<Vector3> vertices;
};

struct ConcaveCollider
{
    Vector3 extents;
    float maxExtent;
    std::vector<Triangle> triangles;
};

struct MeshVertex
{
    Vector3 position = Vector3::Zero();
    Vector3 normal = Vector3::Zero();
    Vector2 uv = Vector2::Zero();
    Vector3 tangent = Vector3::Zero();
    Vector3 bitangent = Vector3::Zero();
    Vector4 boneWeights = Vector4::Zero();
    std::array<uint32_t, 4> boneIds = {0, 0, 0, 0};
};

struct Mesh
{
    Vector3 extents;
    float maxExtent;
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    std::optional<BonesData> bonesData;
};

struct PerVertexBones
{
    std::array<float, 4> boneWeights {-1, -1, -1, -1};
    std::array<uint32_t, 4> boneIds;
};

struct Shader
{
};

struct PositionFrame
{
    float timeInTicks;
    Vector3 position;
};

struct RotationFrame
{
    float timeInTicks;
    Quaternion rotation;
};

struct ScaleFrame
{
    float timeInTicks;
    Vector3 scale;
};

struct SkeletalAnimationFrames
{
    // Vectors are not guaranteed to be the same length. 
    // There could be no rotation data for a frame, for example.
    std::vector<PositionFrame> positionFrames;
    std::vector<RotationFrame> rotationFrames;
    std::vector<ScaleFrame> scaleFrames;
};

struct SkeletalAnimation
{
    std::string name;
    uint32_t durationInTicks;
    float ticksPerSecond;
    std::unordered_map<std::string, SkeletalAnimationFrames> framesPerBone;
};

struct Texture
{
    static constexpr uint32_t numChannels = 4u;

    uint32_t width;
    uint32_t height;
    std::vector<unsigned char> pixelData;
};

struct CubeMap
{
    static constexpr uint32_t numChannels = 4u;

    uint32_t faceSideLength;
    std::vector<unsigned char> pixelData;
};
} // namespace nc::asset
