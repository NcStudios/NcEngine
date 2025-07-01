/**
 * @file Assets.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "AssetType.h"

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

struct ConvexHull
{
    Vector3 extents;
    float maxExtent;
    std::vector<uint8_t> blob;
};

struct MeshCollider
{
    Vector3 extents;
    float maxExtent;
    std::vector<uint8_t> blob;
};

struct MeshVertex
{
    Vector3 position = Vector3::Zero();
    Vector3 normal = Vector3::Zero();
    Vector2 uv = Vector2::Zero();
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

/**
 * 
 *
 * 
  struct ShapeKeyAnimation
{
    std::string name;
    uint32_t durationInTicks;
    float ticksPerSecond;
    uint32_t shapeKeyCount;
    std::vector<std::vector<Vector3>> positionFrames;
};


shapeKeyAnimation.name=Move
shapeKeyAnimation.durationInTicks=416
shapeKeyAnimation.ticksPerSecond =1000.00000
shapeKeyAnimation.shapeKeyCount=2
shapeKeyAnimation.positionFrames

{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=1.00000000 z=-1.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=1.00000000 y=1.00000000 z=2.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=1.00000000 z=2.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }
{x=-1.00000000 y=1.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=2.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=-1.00000000 y=1.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }


{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=2.00000000 z=-1.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=1.00000000 y=1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=1.00000000 z=1.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }
{x=-1.00000000 y=2.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=1.00000000 }
{x=1.00000000 y=-1.00000000 z=-1.00000000 }
{x=-1.00000000 y=-1.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=1.00000000 }
{x=-1.00000000 y=1.00000000 z=1.00000000 }
{x=-1.00000000 y=2.00000000 z=-1.00000000 }
{x=1.00000000 y=1.00000000 z=-1.00000000 }
 * 
 * 
 */


/*
Shape Key Animation:
Shape Key[]:
    Shape Key:
        VertexPosition[]:
            VertexPosition:
                X (float)
                Y (float)
                Z (float)

RGB[][]
XYZ, XYZ, XYZ.... (Num vertices in a mesh)
(Each row = mesh)

*/ 

struct TextureSubResource
{
    uint32_t width = 0u;
    uint32_t height = 0u;
    std::vector<unsigned char> pixelData = {};
};

template<typename T>
struct Texture
{
    TextureFormat format = TextureFormat::UNKNOWN;
    uint32_t width = 0u;
    uint32_t height = 0u;
    uint32_t numChannels = 4u;
    std::vector<T> pixelData = {};
    std::vector<TextureSubResource> mipmaps = {};
};

struct ShapeKeyAnimation
{
    Texture<float> animation = {};
    float durationInSeconds = 0.0f;
    uint32_t numShapeKeys = 0u;
};

struct CubeMap
{
    static constexpr uint32_t numChannels = 4u;

    TextureFormat format = TextureFormat::UNKNOWN;
    uint32_t faceSideLength = 0u;
    std::array<std::vector<unsigned char>, 6> faces = {};
};
} // namespace nc::asset
