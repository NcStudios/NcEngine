#include "GeometryConverter.h"
#include "analysis/GeometryAnalysis.h"
#include "analysis/Sanitize.h"
#include "optimizer/MeshOptimization.h"
#include "utility/Path.h"
#include "utility/Log.h"

#include "ncasset/Assets.h"
#include "ncasset/AssetType.h"
#include "ncjolt/JoltApi.h"
#include "ncjolt/ShapeUtility.h"
#include "ncjolt/Profiler.inl"
#include "ncutility/NcError.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fmt/format.h"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"

#include <algorithm>
#include <array>
#include <ctime>
#include <iostream>
#include <queue>
#include <span>
#include <unordered_map>
#include <ranges>

namespace
{
constexpr auto concaveColliderFlags = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
constexpr auto hullColliderFlags = concaveColliderFlags | aiProcess_JoinIdenticalVertices;
constexpr auto meshFlags = hullColliderFlags | aiProcess_GenNormals;
constexpr auto skeletalAnimationFlags = meshFlags | aiProcess_LimitBoneWeights;
const auto supportedFileExtensions = std::array<std::string, 3> {".fbx", ".obj", ".glb"};

auto ReadScene(const std::filesystem::path& path, Assimp::Importer* importer, unsigned flags) -> const aiScene*
{
    if (!nc::convert::ValidateInputFileExtension(path, supportedFileExtensions))
    {
        throw nc::NcError("Invalid input file: ", path.string());
    }

    if (!importer->ValidateFlags(flags))
    {
        throw nc::NcError("Unsupported import flags");
    }

    auto scene = importer->ReadFile(path.string(), flags);
    if (!scene)
    {
        throw nc::NcError(fmt::format("Assimp failure: {} \n    file: {}",
            importer->GetErrorString(), path.string()
        ));
    }

    if (scene->mNumMeshes == 0)
    {
        throw nc::NcError("Scene contains no mesh data\n    file: ", path.string());
    }

    return scene;
}

template<class T>
[[noreturn]] void SubResourceErrorHandler(const std::string& resource, std::span<T> items)
{
    auto ss = std::ostringstream{};
    ss << "A sub-resource name was provided but no sub-resource was found by that name: " << resource << ".\nNo asset will created. Found sub-resources: \n";
    std::ranges::for_each(items, [&ss](auto&& item){ ss << item->mName.C_Str() << ", "; });
    throw nc::NcError(ss.str());
}

auto GetMeshFromScene(const aiScene* scene, const std::string_view subResourceName) -> aiMesh*
{
    auto target = aiString{subResourceName.data()};
    auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);
    auto pos = std::ranges::find(meshes, target, [](auto&& m) { return m->mName; });
    if (pos != std::cend(meshes))
    {
        return *pos;
    }

    SubResourceErrorHandler<aiMesh*>(subResourceName.data(), meshes);
}

auto GetAnimationFromScene(const aiScene* scene, const std::string_view subResourceName) -> aiAnimation*
{
    auto target = aiString{subResourceName.data()};
    auto animations = std::span(scene->mAnimations, scene->mNumAnimations);
    auto pos = std::ranges::find(animations, target, [](auto&& m) { return m->mName; });
    if (pos != std::cend(animations))
    {
        return *pos;
    }

    SubResourceErrorHandler<aiAnimation*>(subResourceName.data(), animations);
}

/**
 * @brief FBX exports animMesh mesh names with a trailing "*n" where n is an index. Remove this as it doesn't accurately reference the mesh name.
 * Intentionally copying to a string
 */
auto CleanAnimMeshMeshName(std::string_view name) -> std::string
{
    auto reversedName = name | std::views::reverse;

    // Find the first character that is not a digit, starting from the end.
    auto pos = std::ranges::find_if_not(reversedName, [](char c)
    {
        return std::isdigit(static_cast<unsigned char>(c));
    });

    if (pos != reversedName.end() && *pos == '*')
    {
        auto trailLength = std::ranges::distance(reversedName.begin(), ++pos);
        return std::string(name.substr(0, name.size() - trailLength));
    }

    // No trail found.
    return std::string(name);
}

auto GetAnimMeshMeshFromAnimation(const aiScene* scene, const aiAnimation* animation) -> aiMesh*
{
    NC_ASSERT(animation != nullptr, "Animation cannot be nullptr.");
    NC_ASSERT(animation->mNumMorphMeshChannels != 0, "No shape keys found in scene.");
    NC_ASSERT(scene->mNumMeshes != 0, "No meshes found in scene.");

    auto animationName = std::string_view(animation->mMorphMeshChannels[0]->mName.C_Str());
    auto cleanedAnimationName = CleanAnimMeshMeshName(animationName);
    return GetMeshFromScene(scene, cleanedAnimationName);
}

auto GetAnimMeshesFromScene(const aiScene* scene, const std::string_view subResourceName) -> aiAnimMesh**
{
    NC_ASSERT(scene->mNumMeshes != 0, "No meshes found in scene.");

    auto target = aiString{subResourceName.data()};
    auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);
    auto pos = std::ranges::find(meshes, target, [](auto&& m) { return m->mName; });

    if (pos == std::cend(meshes))
    {
        SubResourceErrorHandler<aiMesh*>(subResourceName.data(), meshes);
    }

    auto* mesh = *pos;

    NC_ASSERT(mesh->mNumAnimMeshes != 0, "No anim meshes found associated with the mesh.");

    return mesh->mAnimMeshes;
}

auto GetFromScene(const aiScene* scene, const std::optional<std::string>& subResourceName = std::nullopt) -> aiAnimation*
{
    NC_ASSERT(scene->mNumAnimations != 0, "No animations found in scene.");

    if (!subResourceName.has_value())
    {
        return scene->mAnimations[0];
    }

    auto target = aiString{subResourceName.value()};
    auto animations = std::span(scene->mAnimations, scene->mNumAnimations);
    auto pos = std::ranges::find(animations, target, [](auto&& m) { return m->mName; });
    if (pos != std::cend(animations))
    {
        return *pos;
    }

    SubResourceErrorHandler<aiAnimation*>(subResourceName.value(), animations);
}

auto ToVector3(const aiVector3D& in) -> nc::Vector3
{
    return nc::Vector3{in.x, in.y, in.z};
}

auto ViewVertices(const aiMesh* mesh) -> std::span<const aiVector3D>
{
    return {mesh->mVertices, mesh->mNumVertices};
}

auto ViewVertices(const aiAnimMesh* animMesh) -> std::span<const aiVector3D>
{
    return {animMesh->mVertices, animMesh->mNumVertices};
}

auto ViewFaces(const aiMesh* mesh) -> std::span<const aiFace>
{
    return {mesh->mFaces, mesh->mNumFaces};
}

auto ConvertToVertices(std::span<const aiVector3D> vertices) -> std::vector<nc::Vector3>
{
    auto out = std::vector<nc::Vector3>{};
    out.reserve(vertices.size());
    std::ranges::transform(vertices, std::back_inserter(out), [](auto&& vertex)
    {
        return ::ToVector3(vertex);
    });

    return out;
}

auto ConvertToIndices(std::span<const aiFace> faces) -> std::vector<uint32_t>
{
    auto out = std::vector<uint32_t>{};
    out.reserve(faces.size() * 3);
    for (const auto& face : faces)
    {
        NC_ASSERT(face.mNumIndices == 3, "Found non-triangular face.");
        out.push_back(face.mIndices[0]);
        out.push_back(face.mIndices[1]);
        out.push_back(face.mIndices[2]);
    }

    return out;
}

auto ConvertToTriangles(std::span<const aiFace> faces, std::span<const aiVector3D> vertices) -> std::vector<nc::Triangle>
{
    auto out = std::vector<nc::Triangle>{};
    out.reserve(faces.size());
    std::ranges::transform(faces, std::back_inserter(out), [&vertices](auto&& face)
    {
        NC_ASSERT(face.mNumIndices == 3, "Found non-triangular face.");
        return nc::Triangle{
            ::ToVector3(vertices[face.mIndices[0]]),
            ::ToVector3(vertices[face.mIndices[1]]),
            ::ToVector3(vertices[face.mIndices[2]])
        };
    });

    return out;
}

auto ConvertToXMMATRIX(const aiMatrix4x4* inputMatrix) -> DirectX::XMMATRIX
{
    return DirectX::XMMatrixTranspose(DirectX::XMMATRIX
    {
        inputMatrix->a1, inputMatrix->a2, inputMatrix->a3, inputMatrix->a4,
        inputMatrix->b1, inputMatrix->b2, inputMatrix->b3, inputMatrix->b4,
        inputMatrix->c1, inputMatrix->c2, inputMatrix->c3, inputMatrix->c4,
        inputMatrix->d1, inputMatrix->d2, inputMatrix->d3, inputMatrix->d4
    });
}

auto GetBoneWeights(const aiMesh* mesh) -> std::unordered_map<uint32_t, nc::asset::PerVertexBones>
{
    auto perVertexBones = std::unordered_map<uint32_t, nc::asset::PerVertexBones>();

    // Iterate through all the bones in the mesh
    for (auto boneIndex = 0u; boneIndex < mesh->mNumBones; boneIndex++)
    {
        auto* currentBone = mesh->mBones[boneIndex];

        if (currentBone->mNumWeights ==1)
        {
            if (nc::FloatEqual(currentBone->mWeights[0].mWeight, 0.0f))
            {
                continue;
            }
        }
        
        // Iterate through all the vertex weights each bone has
        for (auto boneWeightIndex = 0u; boneWeightIndex < currentBone->mNumWeights; boneWeightIndex++)
        {
            auto vertexId = currentBone->mWeights[boneWeightIndex].mVertexId;
            auto& currentPerVertexBones = perVertexBones[vertexId];
            if (currentPerVertexBones.boneWeights[3] != -1)
            {
                throw nc::NcError("Cannot import a mesh with more than four bones influencing any single vertex.");
            }

            // Create a mapping from vertex to collection of bone weights and IDs
            for (auto i = 0u; i < currentPerVertexBones.boneIds.size(); i++)
            {
                if (currentPerVertexBones.boneWeights[i] == -1)
                {
                    currentPerVertexBones.boneIds[i] = boneIndex;
                    currentPerVertexBones.boneWeights[i] = currentBone->mWeights[boneWeightIndex].mWeight;
                    break;
                }
            }

            if (currentPerVertexBones.boneWeights[3] != -1)
            {
                auto boneWeightTotal = 
                currentPerVertexBones.boneWeights[0] +
                currentPerVertexBones.boneWeights[1] + 
                currentPerVertexBones.boneWeights[2] + 
                currentPerVertexBones.boneWeights[3];

                if (!nc::FloatEqual(boneWeightTotal, 1.0f))
                {
                    throw nc::NcError("The sum of bone weights affecting each vertex must equal 1. Current weight: ", std::to_string(boneWeightTotal));
                }
            }
        }
    }
    return perVertexBones;
}

/**
 * @brief Converts the given aiNode* tree of bone spaces into a flattened vector where all siblings are contiguous.
 * Tree:
 * A
 *    A1
 *       A1A   A1B
 *    B1
 * 
 * Vector:
 * [A, A1, B1, A1A, A1B]
 */
auto GetBoneSpaceToParentSpaceMatrices(const aiNode* inputNode) -> std::vector<nc::asset::BoneSpaceToParentSpace>
{
    auto out = std::vector<nc::asset::BoneSpaceToParentSpace>();
    auto unprocessedNodes = std::queue<const aiNode*>{};

    if (!inputNode)
    {
        return out;
    }

    unprocessedNodes.push(inputNode);
    const aiNode* currentNode = nullptr;

    while (!unprocessedNodes.empty())
    {
        currentNode = unprocessedNodes.front();

        // Get the index of the next available slot after the current node and it's children have been placed. (Siblings are always contiguous.)
        auto nextAvailableSlot = static_cast<uint32_t>(unprocessedNodes.size() + out.size());
        auto boneSpaceToParentSpace = nc::asset::BoneSpaceToParentSpace
        {
            .boneName = std::string(currentNode->mName.data),
            .transformationMatrix = ConvertToXMMATRIX(&currentNode->mTransformation),
            .numChildren = currentNode->mNumChildren,
            .indexOfFirstChild = nextAvailableSlot
        };

        unprocessedNodes.pop();
        out.push_back(std::move(boneSpaceToParentSpace));

        for (auto childIndex = 0u; childIndex < currentNode->mNumChildren; childIndex++)
        {
            unprocessedNodes.push(currentNode->mChildren[childIndex]);
        }
    }
    return out;
}

auto GetVertexToBoneSpaceMatrices(const aiMesh* mesh) -> std::vector<nc::asset::VertexSpaceToBoneSpace>
{
    auto out = std::vector<nc::asset::VertexSpaceToBoneSpace>();
    out.reserve(mesh->mNumBones);

    for (auto boneIndex = 0u; boneIndex < mesh->mNumBones; boneIndex++)
    {
        auto* currentBone = mesh->mBones[boneIndex];
        auto boneName = std::string(currentBone->mName.data);
        out.emplace(out.begin() + boneIndex, std::string{currentBone->mName.data}, ConvertToXMMATRIX(&currentBone->mOffsetMatrix));
    }
    return out;
}

auto GetBoneMapping(const std::vector<nc::asset::VertexSpaceToBoneSpace>& vertexSpaceToBoneSpaceMatrices) -> std::unordered_map<std::string, uint32_t>
{
    auto boneMapping = std::unordered_map<std::string, uint32_t>{};
    boneMapping.reserve(vertexSpaceToBoneSpaceMatrices.size());

    for (auto i = 0u; i < vertexSpaceToBoneSpaceMatrices.size(); i++)
    {
        boneMapping.emplace(vertexSpaceToBoneSpaceMatrices[i].boneName, i);
    }

    return boneMapping;
}

auto GetBonesData(const aiMesh* mesh, const aiNode* rootNode) -> nc::asset::BonesData
{
    auto vertexSpaceToBoneSpaces = GetVertexToBoneSpaceMatrices(mesh);
    auto boneSpaceToParentSpaces = GetBoneSpaceToParentSpaceMatrices(rootNode);
    auto boneMapping = GetBoneMapping(vertexSpaceToBoneSpaces);

    return nc::asset::BonesData
    {
        std::move(boneMapping),
        std::move(vertexSpaceToBoneSpaces),
        std::move(boneSpaceToParentSpaces)
    };
}

auto ConvertToMeshVertices(const aiMesh* mesh) -> std::vector<nc::asset::MeshVertex>
{
    NC_ASSERT(static_cast<bool>(mesh->mNormals) &&
              static_cast<bool>(mesh->mTextureCoords),
              "Not all required data was generated for mesh conversion");
    auto out = std::vector<nc::asset::MeshVertex>{};
    const auto nVertices = mesh->mNumVertices;
    out.reserve(nVertices);

    if (mesh->HasBones())
    {
        const auto perVertexBones = GetBoneWeights(mesh);
        for (auto i = 0u; i < nVertices; ++i)
        {
            const auto& uv = mesh->mTextureCoords[0][i];
            const auto& boneData = perVertexBones.at(i);
            const auto boneWeights = nc::Vector4(
                boneData.boneWeights[0],
                boneData.boneWeights[1],
                boneData.boneWeights[2],
                boneData.boneWeights[3]
            );

            out.emplace_back(
                ToVector3(mesh->mVertices[i]),
                ToVector3(mesh->mNormals[i]),
                nc::Vector2{uv.x, uv.y},
                boneWeights,
                boneData.boneIds
            );
        }
        return out;
    }

    for (auto i = 0u; i < nVertices; ++i)
    {
        const auto& uv = mesh->mTextureCoords[0][i];
        out.emplace_back(
            ToVector3(mesh->mVertices[i]),
            ToVector3(mesh->mNormals[i]),
            nc::Vector2{uv.x, uv.y},
            nc::Vector4{-1, -1, -1, -1},
            std::array<uint32_t, 4>{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}
        );
    }
    return out;
}

auto ConvertToSkeletalAnimation(const aiAnimation* animationClip) -> nc::asset::SkeletalAnimation
{
    auto skeletalAnimation = nc::asset::SkeletalAnimation{};
    skeletalAnimation.name = std::string(animationClip->mName.C_Str());
    skeletalAnimation.ticksPerSecond = animationClip->mTicksPerSecond == 0 ? 25.0f : static_cast<float>(animationClip->mTicksPerSecond); // Ticks per second is not required to be set in animation software.
    skeletalAnimation.durationInTicks = static_cast<uint32_t>(animationClip->mDuration);
    skeletalAnimation.framesPerBone.reserve(animationClip->mNumChannels);

    // A single channel represents one bone and all of its transformations for the animation clip.
    for (const auto* channel : std::span(animationClip->mChannels, animationClip->mNumChannels))
    {
        auto frames = nc::asset::SkeletalAnimationFrames{};
        frames.positionFrames.reserve(channel->mNumPositionKeys);
        frames.rotationFrames.reserve(channel->mNumRotationKeys);
        frames.scaleFrames.reserve(channel->mNumScalingKeys);
        
        for (const auto& positionKey : std::span(channel->mPositionKeys, channel->mNumPositionKeys))
        {
            frames.positionFrames.emplace_back(static_cast<float>(positionKey.mTime), nc::Vector3(positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z));
        }

        for (const auto& rotationKey : std::span(channel->mRotationKeys, channel->mNumRotationKeys))
        {
            frames.rotationFrames.emplace_back(static_cast<float>(rotationKey.mTime), nc::Quaternion(rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z, rotationKey.mValue.w));
        }

        for (const auto& scaleKey : std::span(channel->mScalingKeys, channel->mNumScalingKeys))
        {
            frames.scaleFrames.emplace_back(static_cast<float>(scaleKey.mTime), nc::Vector3(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z));
        }
        skeletalAnimation.framesPerBone.emplace(std::string(channel->mNodeName.C_Str()), std::move(frames));
    }
    return skeletalAnimation;
}


/*


// Data Structure of aiAnimation for Blend Shapes:

/*
aiAnimation:
    mNumMorphMeshChannels: 1
    mMorphMeshChannels*: aiMeshMorphAnim []

aiMeshMorphAnim:
    mName: "Flag" (Name of Mesh)
    mNumKeys: 241
    mKeys*: aiMeshMorphKey []

aiMeshMorphKey:
    mTime: 41.666667
    mValues: int[]
    mWeights: double[]
    mNumValuesAndWeights: 25

*/

// Data Structure of aiAnimMesh for Blend Shapes:
/*
aiScene
    mNumMeshes: 1
    mMeshes: aiMesh[]

aiMesh
    mNumAnimMeshes: 25
    mAnimMeshes: aiAnimMesh[]
    mMethod: aiMorphingMethod
        aiMorphingMethod_Unknown

aiAnimMesh
    mName: ShapeKey.1
    mNumVertices: 1089
    mVertices: aiVector3t<float>
    mWeight: float



/**


Data Structure:

struct ShapeKeyAnimation
{
    std::string name;
    uint32_t durationInTicks;
    float ticksPerSecond;
    std::vector<std::vector<PositionFrame>> positionFrames; (1089 inner vector) (1 inner vector per second/ or per tick)
};

uilding shapekey-animation: C:\Source\NcEngine\sample\assets\.\nca\shapekey_animation\flag.nca ()
mTime: 41.6667
mValue: 0, mWeight: 1, 
mValue: 1, mWeight: 0, 
mValue: 2, mWeight: 0, 
mValue: 3, mWeight: 0, 
mValue: 4, mWeight: 0, 
mValue: 5, mWeight: 0, 
mValue: 6, mWeight: 0, 
mValue: 7, mWeight: 0, 
mValue: 8, mWeight: 0, 
mValue: 9, mWeight: 0, 
mValue: 10, mWeight: 0, 
mValue: 11, mWeight: 0, 
mValue: 12, mWeight: 0, 
mValue: 13, mWeight: 0, 
mValue: 14, mWeight: 0, 
mValue: 15, mWeight: 0, 
mValue: 16, mWeight: 0, 
mValue: 17, mWeight: 0, 
mValue: 18, mWeight: 0, 
mValue: 19, mWeight: 0, 
mValue: 20, mWeight: 0, 
mValue: 21, mWeight: 0, 
mValue: 22, mWeight: 0, 
mValue: 23, mWeight: 0, 
mValue: 24, mWeight: 0, 
2

mTime: 83.3333
mValue: 0, mWeight: 0.9, 
mValue: 1, mWeight: 0.1, 
mValue: 2, mWeight: 0, 
mValue: 3, mWeight: 0, 
mValue: 4, mWeight: 0, 
mValue: 5, mWeight: 0, 
mValue: 6, mWeight: 0, 
mValue: 7, mWeight: 0, 
mValue: 8, mWeight: 0, 
mValue: 9, mWeight: 0, 
mValue: 10, mWeight: 0, 
mValue: 11, mWeight: 0, 
mValue: 12, mWeight: 0, 
mValue: 13, mWeight: 0, 
mValue: 14, mWeight: 0, 
mValue: 15, mWeight: 0, 
mValue: 16, mWeight: 0, 
mValue: 17, mWeight: 0, 
mValue: 18, mWeight: 0, 
mValue: 19, mWeight: 0, 
mValue: 20, mWeight: 0, 
mValue: 21, mWeight: 0, 
mValue: 22, mWeight: 0, 
mValue: 23, mWeight: 0, 
mValue: 24, mWeight: 0, 
2


std::vector<nc::Vector3> vertexOffsets

In Vertex Shader
    Get my vertex offset from vector
    Add my vertex offset to my position
 */

 /*

struct ShapeKeyAnimation
{
    std::string name;
    float durationInSeconds;
    Texture animation = {};
    // animation {0.1,  2.0, 1.3,  1.0, 3.5, 1.0}
    //              { x,   y,   z,    x,   y,   z}
    // VK_FORMAT_R32_SFLOAT 
    // 
};

struct Texture
{

    TextureFormat format = TextureFormat::UNKNOWN;
    uint32_t width = 0u;
    uint32_t height = 0u;
    uint32_t numChannels = 4u;
    std::vector<unsigned char> pixelData = {};
    std::vector<TextureSubResource> mipmaps = {};
};
 */

auto ConvertToShapeKeyAnimation(const aiAnimation* animationClip, const aiMesh* mesh) -> nc::asset::ShapeKeyAnimation
{
    NC_ASSERT(mesh, "Mesh cannot be null.");
    NC_ASSERT(animationClip, "Animation clip cannot be null.");

    auto numShapeKeys = mesh->mNumAnimMeshes;
    NC_ASSERT(numShapeKeys != 0, "No shape keys detected in the mesh.");

    auto shapeKeyAnimation = nc::asset::ShapeKeyAnimation{};

    auto positionData = std::vector<float>{};
    positionData.reserve(mesh->mAnimMeshes[0]->mNumVertices * 3u * numShapeKeys); // Each vertex has 3 floats, flattening nested array of vertices * shapekeys

    for (const auto& animMesh : std::span(mesh->mAnimMeshes, mesh->mNumAnimMeshes))
    {
        for (const auto& vertex : ::ViewVertices(animMesh))
        {
            positionData.push_back(vertex.x);
            positionData.push_back(vertex.y);
            positionData.push_back(vertex.z);
        }
    }

    shapeKeyAnimation.animation = nc::asset::Texture<float>
    {
        .format = nc::asset::TextureFormat::R32_SFLOAT,
        .width = mesh->mAnimMeshes[0]->mNumVertices * 3u, // Each vertex has 3 floats
        .height = numShapeKeys,
        .numChannels = 1u, // Storing everything in R
        .pixelData = std::move(positionData)
    };

    auto durationInTicks = static_cast<uint32_t>(animationClip->mDuration);
    auto ticksPerSecond = animationClip->mTicksPerSecond == 0 ? 25.0f : static_cast<float>(animationClip->mTicksPerSecond); // Ticks per second is not required to be set in animation software.
    shapeKeyAnimation.durationInSeconds = durationInTicks / ticksPerSecond;

    shapeKeyAnimation.numShapeKeys = numShapeKeys;

    return shapeKeyAnimation;
}
} // anonymous namespace

/*
8333
1000.00000

{x=-1.00000000 y=0.00000000 z=-1.01383543 }
{x=1.00000000 y=0.00000000 z=-1.01383543 }
{x=1.00000000 y=0.00000000 z=1.00000000 }
{x=-1.00000000 y=0.184691206 z=1.00000000 }


{x=-1.00000000 y=0.0596593395 z=-1.00000000 }
{x=1.00000000 y=0.00000000 z=-1.00000000 }
{x=1.00000000 y=0.00000000 z=3.00000000 }
{x=-1.00000000 y=0.00000000 z=1.00000000 }


{x=-1.00999999 y=0.00000000 z=-1.00999999 }
{x=1.00999999 y=-0.106265679 z=-1.00999999 }
{x=1.00999999 y=0.00000000 z=1.04837060 }
{x=-1.00999999 y=0.00000000 z=1.04837060 }
*/

namespace nc::convert
{
class GeometryConverter::impl
{
    public:
        auto ImportMeshCollider(const std::filesystem::path& path) -> asset::MeshCollider
        {
            const auto mesh = ::ReadScene(path, &m_importer, concaveColliderFlags)->mMeshes[0];

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto triangles = ::ConvertToTriangles(::ViewFaces(mesh), ::ViewVertices(mesh));
            if(auto count = Sanitize(triangles))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            const auto shape = jolt::BuildMeshShape(triangles);
            return asset::MeshCollider{
                GetMeshVertexExtents(triangles),
                FindFurthestDistanceFromOrigin(triangles),
                jolt::SerializeShape(*shape)
            };
        }

        auto ImportConvexHull(const std::filesystem::path& path) -> asset::ConvexHull
        {
            const auto mesh = ::ReadScene(path, &m_importer, hullColliderFlags)->mMeshes[0];

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto convertedVertices = ::ConvertToVertices(::ViewVertices(mesh));
            if(auto count = Sanitize(convertedVertices))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            const auto shape = jolt::BuildConvexHull(convertedVertices);
            return asset::ConvexHull{
                GetMeshVertexExtents(convertedVertices),
                FindFurthestDistanceFromOrigin(convertedVertices),
                jolt::SerializeShape(*shape)
            };
        }

        auto ImportMesh(const std::filesystem::path& path, const std::optional<std::string>& subResourceName, bool optimize) -> asset::Mesh
        {
            const auto scene = ::ReadScene(path, &m_importer, meshFlags);

            NC_ASSERT(scene->mNumMeshes != 0, "No meshes found in scene.");

            aiMesh* mesh = nullptr;
            if (!subResourceName.has_value())
            {
                mesh = scene->mMeshes[0];
            }
            else
            {
                mesh = GetMeshFromScene(scene, subResourceName.value());
            }

            NC_ASSERT(mesh, "No meshes found in scene.");

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto convertedVertices = ::ConvertToMeshVertices(mesh);
            if(auto count = Sanitize(convertedVertices))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            auto convertedIndices = ::ConvertToIndices(::ViewFaces(mesh));
            auto [processedVertices, processedIndices] = optimize
                ? OptimizeMesh(convertedVertices, convertedIndices)
                : OptimizedMesh{std::move(convertedVertices), std::move(convertedIndices)};

            return asset::Mesh{
                GetMeshVertexExtents(processedVertices),
                FindFurthestDistanceFromOrigin(processedVertices),
                std::move(processedVertices),
                std::move(processedIndices),
                GetBonesData(mesh, scene->mRootNode)
            };
        }

        auto ImportSkeletalAnimation(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::SkeletalAnimation
        {
            const auto scene = ::ReadScene(path, &m_importer, skeletalAnimationFlags);

            NC_ASSERT(scene->mNumAnimations != 0, "No animations found in scene.");

            if (!subResourceName.has_value())
            {
                return ::ConvertToSkeletalAnimation(scene->mAnimations[0]);
            }

            auto animation = GetAnimationFromScene(scene, subResourceName.value());
            return ::ConvertToSkeletalAnimation(animation);
        }

        auto ImportShapeKeyAnimation(const std::filesystem::path& path, const std::string_view subResourceName) -> asset::ShapeKeyAnimation
        {
            const auto scene = ::ReadScene(path, &m_importer, meshFlags);

            NC_ASSERT(scene->mNumAnimations != 0, "No animations found in scene.");

            const auto* animation = GetAnimationFromScene(scene, subResourceName);
            const auto* mesh = GetAnimMeshMeshFromAnimation(scene, animation);
            
            return ::ConvertToShapeKeyAnimation(animation, mesh);
        }

    private:
        Assimp::Importer m_importer;
        jolt::JoltApi m_joltApi;
};

GeometryConverter::GeometryConverter()
    : m_impl{std::make_unique<GeometryConverter::impl>()}
{
}

GeometryConverter::~GeometryConverter() noexcept = default;

auto GeometryConverter::ImportMeshCollider(const std::filesystem::path& path) -> asset::MeshCollider
{
    return m_impl->ImportMeshCollider(path);
}

auto GeometryConverter::ImportConvexHull(const std::filesystem::path& path) -> asset::ConvexHull
{
    return m_impl->ImportConvexHull(path);
}

auto GeometryConverter::ImportMesh(const std::filesystem::path& path, const std::optional<std::string>& subResourceName, bool optimize) -> asset::Mesh
{
    return m_impl->ImportMesh(path, subResourceName, optimize);
}

auto GeometryConverter::ImportSkeletalAnimation(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::SkeletalAnimation
{
    return m_impl->ImportSkeletalAnimation(path, subResourceName);
}

auto GeometryConverter::ImportShapeKeyAnimation(const std::filesystem::path& path, const std::string_view subResourceName) -> asset::ShapeKeyAnimation
{
    return m_impl->ImportShapeKeyAnimation(path, subResourceName);
}

} // namespace nc::convert
