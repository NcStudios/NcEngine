#include "GeometryConverter.h"
#include "analysis/GeometryAnalysis.h"
#include "analysis/Sanitize.h"
#include "utility/Path.h"
#include "utility/Log.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fmt/format.h"
#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <queue>
#include <span>
#include <unordered_map>
#include <ctime>

namespace
{
constexpr auto concaveColliderFlags = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
constexpr auto hullColliderFlags = concaveColliderFlags | aiProcess_JoinIdenticalVertices;
constexpr auto meshFlags = hullColliderFlags | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
constexpr auto skeletalAnimationFlags = meshFlags | aiProcess_LimitBoneWeights;
const auto supportedFileExtensions = std::array<std::string, 2> {".fbx", ".obj"};

auto ReadFbx(const std::filesystem::path& path, Assimp::Importer* importer, unsigned flags) -> const aiScene*
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
        throw nc::NcError("Fbx contains no mesh data\n    file: ", path.string());
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

auto GetMeshFromScene(const aiScene* scene, const std::optional<std::string>& subResourceName = std::nullopt) -> aiMesh*
{
    NC_ASSERT(scene->mNumMeshes != 0, "No meshes found in scene.");

    if (!subResourceName.has_value())
    {
        return scene->mMeshes[0];
    }

    auto target = aiString{subResourceName.value()};
    auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);
    auto pos = std::ranges::find(meshes, target, [](auto&& m) { return m->mName; });
    if (pos != std::cend(meshes))
    {
        return *pos;
    }

    SubResourceErrorHandler<aiMesh*>(subResourceName.value(), meshes);
}

auto GetAnimationFromMesh(const aiScene* scene, const std::optional<std::string>& subResourceName = std::nullopt) -> aiAnimation*
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
              static_cast<bool>(mesh->mTextureCoords) &&
              static_cast<bool>(mesh->mTangents) &&
              static_cast<bool>(mesh->mBitangents),
              "Not all required data was generated for mesh conversion");
    auto out = std::vector<nc::asset::MeshVertex>{};
    const auto nVertices = mesh->mNumVertices;
    out.reserve(nVertices);

    if (mesh->HasBones())
    {
        const auto perVertexBones = GetBoneWeights(mesh);
        for (auto i = 0u; i < nVertices; ++i)
        {
            const auto uv = mesh->mTextureCoords[0][i];
            auto boneWeights = nc::Vector4(perVertexBones.at(i).boneWeights[0],
                                        perVertexBones.at(i).boneWeights[1],
                                        perVertexBones.at(i).boneWeights[2],
                                        perVertexBones.at(i).boneWeights[3]);
            out.emplace_back(
                ToVector3(mesh->mVertices[i]), ToVector3(mesh->mNormals[i]), nc::Vector2{uv.x, uv.y},
                ToVector3(mesh->mTangents[i]), ToVector3(mesh->mBitangents[i]), boneWeights, perVertexBones.at(i).boneIds
            );
        }
        return out;
    }

    for (auto i = 0u; i < nVertices; ++i)
    {
        const auto uv = mesh->mTextureCoords[0][i];
        out.emplace_back(
            ToVector3(mesh->mVertices[i]), ToVector3(mesh->mNormals[i]), nc::Vector2{uv.x, uv.y},
            ToVector3(mesh->mTangents[i]), ToVector3(mesh->mBitangents[i]), nc::Vector4{-1, -1, -1, -1}, std::array<uint32_t, 4>{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}
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
} // anonymous namespace

namespace nc::convert
{
class GeometryConverter::impl
{
    public:
        auto ImportConcaveCollider(const std::filesystem::path& path) -> asset::ConcaveCollider
        {
            const auto mesh = ::ReadFbx(path, &m_importer, concaveColliderFlags)->mMeshes[0];

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto triangles = ::ConvertToTriangles(::ViewFaces(mesh), ::ViewVertices(mesh));
            if(auto count = Sanitize(triangles))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            return asset::ConcaveCollider{
                GetMeshVertexExtents(triangles),
                FindFurthestDistanceFromOrigin(triangles),
                std::move(triangles)
            };
        }

        auto ImportHullCollider(const std::filesystem::path& path) -> asset::HullCollider
        {
            const auto mesh = ::ReadFbx(path, &m_importer, hullColliderFlags)->mMeshes[0];

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto convertedVertices = ::ConvertToVertices(::ViewVertices(mesh));
            if(auto count = Sanitize(convertedVertices))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            return asset::HullCollider{
                GetMeshVertexExtents(convertedVertices),
                FindFurthestDistanceFromOrigin(convertedVertices),
                std::move(convertedVertices)
            };
        }

        auto ImportMesh(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::Mesh
        {
            const auto scene = ::ReadFbx(path, &m_importer, meshFlags);
            auto mesh = GetMeshFromScene(scene, subResourceName);

            if (mesh->mNumVertices == 0)
            {
                throw nc::NcError("No vertices found for: ", path.string());
            }

            auto convertedVertices = ::ConvertToMeshVertices(mesh);
            if(auto count = Sanitize(convertedVertices))
            {
                LOG("Warning: Bad values detected in mesh. {} values have been set to 0.", count);
            }

            return asset::Mesh{
                GetMeshVertexExtents(convertedVertices),
                FindFurthestDistanceFromOrigin(convertedVertices),
                std::move(convertedVertices),
                ::ConvertToIndices(::ViewFaces(mesh)),
                GetBonesData(mesh, scene->mRootNode)
            };
        }

        auto ImportSkeletalAnimation(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::SkeletalAnimation
        {
            const auto scene = ::ReadFbx(path, &m_importer, skeletalAnimationFlags);
            auto animation = GetAnimationFromMesh(scene, subResourceName);
            return ::ConvertToSkeletalAnimation(animation);
        }

    private:
        Assimp::Importer m_importer;
};

GeometryConverter::GeometryConverter()
    : m_impl{std::make_unique<GeometryConverter::impl>()}
{
}

GeometryConverter::~GeometryConverter() noexcept = default;

auto GeometryConverter::ImportConcaveCollider(const std::filesystem::path& path) -> asset::ConcaveCollider
{
    return m_impl->ImportConcaveCollider(path);
}

auto GeometryConverter::ImportHullCollider(const std::filesystem::path& path) -> asset::HullCollider
{
    return m_impl->ImportHullCollider(path);
}

auto GeometryConverter::ImportMesh(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::Mesh
{
    return m_impl->ImportMesh(path, subResourceName);
}

auto GeometryConverter::ImportSkeletalAnimation(const std::filesystem::path& path, const std::optional<std::string>& subResourceName) -> asset::SkeletalAnimation
{
    return m_impl->ImportSkeletalAnimation(path, subResourceName);
}

} // namespace nc::convert
