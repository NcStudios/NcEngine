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

auto ImportConcaveCollider(std::istream& data) -> ConcaveCollider
{
    auto [header, asset] = DeserializeConcaveCollider(data);
    return asset;
}

auto ImportConcaveCollider(const std::filesystem::path& ncaPath) -> ConcaveCollider
{
    auto file = ::OpenNca(ncaPath);
    return ImportConcaveCollider(file);
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

auto ImportHullCollider(std::istream& data) -> HullCollider
{
    auto [header, asset] = DeserializeHullCollider(data);
    return asset;
}

auto ImportHullCollider(const std::filesystem::path& ncaPath) -> HullCollider
{
    auto file = ::OpenNca(ncaPath);
    return ImportHullCollider(file);
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
