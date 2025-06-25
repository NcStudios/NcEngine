#include "ncasset/Import.h"
#include "Deserialize.h"

#include "ncutility/NcError.h"

#include <cstring>
#include <fstream>

namespace
{
auto OpenNca(const std::filesystem::path& ncaPath) -> std::ifstream
{
    if (!std::filesystem::is_regular_file(ncaPath))
    {
        throw nc::NcError("File does not exist: ", ncaPath.string());
    }

    auto file = std::ifstream{ncaPath, std::ios::binary};
    if (!file.is_open())
    {
        throw nc::NcError("Could not open file: ", ncaPath.string());
    }

    return file;
}
} // anonymous namespace

namespace nc::asset
{
auto ImportNcaHeader(std::istream& data) -> NcaHeader
{
    return DeserializeHeader(data);
}

auto ImportNcaHeader(const std::filesystem::path& ncaPath) -> NcaHeader
{
    auto file = ::OpenNca(ncaPath);
    return ImportNcaHeader(file);
}

auto ImportAudioClip(std::istream& data) -> AudioClip
{
    auto [header, asset] = DeserializeAudioClip(data);
    return asset;
}

auto ImportAudioClip(const std::filesystem::path& ncaPath) -> AudioClip
{
    auto file = ::OpenNca(ncaPath);
    return ImportAudioClip(file);
}

auto ImportMeshCollider(std::istream& data) -> MeshCollider
{
    auto [header, asset] = DeserializeMeshCollider(data);
    return asset;
}

auto ImportMeshCollider(const std::filesystem::path& ncaPath) -> MeshCollider
{
    auto file = ::OpenNca(ncaPath);
    return ImportMeshCollider(file);
}

auto ImportCubeMap(std::istream& data) -> CubeMap
{
    auto [header, asset] = DeserializeCubeMap(data);
    return asset;
}

auto ImportCubeMap(const std::filesystem::path& ncaPath) -> CubeMap
{
    auto file = ::OpenNca(ncaPath);
    return ImportCubeMap(file);
}

auto ImportConvexHull(std::istream& data) -> ConvexHull
{
    auto [header, asset] = DeserializeConvexHull(data);
    return asset;
}

auto ImportConvexHull(const std::filesystem::path& ncaPath) -> ConvexHull
{
    auto file = ::OpenNca(ncaPath);
    return ImportConvexHull(file);
}

auto ImportMesh(std::istream& data) -> Mesh
{
    auto [header, asset] = DeserializeMesh(data);
    return asset;
}

auto ImportMesh(const std::filesystem::path& ncaPath) -> Mesh
{
    auto file = ::OpenNca(ncaPath);
    return ImportMesh(file);
}

auto ImportShapeKeyAnimation(std::istream& data) -> ShapeKeyAnimation
{
    auto [header, asset] = DeserializeShapeKeyAnimation(data);
    return asset;
}

auto ImportShapeKeyAnimation(const std::filesystem::path& ncaPath) -> ShapeKeyAnimation
{
    auto file = ::OpenNca(ncaPath);
    return ImportShapeKeyAnimation(file);
}

auto ImportSkeletalAnimation(std::istream& data) -> SkeletalAnimation
{
    auto [header, asset] = DeserializeSkeletalAnimation(data);
    return asset;
}

auto ImportSkeletalAnimation(const std::filesystem::path& ncaPath) -> SkeletalAnimation
{
    auto file = ::OpenNca(ncaPath);
    return ImportSkeletalAnimation(file);
}

auto ImportTexture(std::istream& data) -> Texture
{
    auto [header, asset] = DeserializeTexture(data);
    return asset;
}

auto ImportTexture(const std::filesystem::path& ncaPath) -> Texture
{
    auto file = ::OpenNca(ncaPath);
    return ImportTexture(file);
}
} // namespace nc::asset
