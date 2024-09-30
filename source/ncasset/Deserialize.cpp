#include "Deserialize.h"
#include "ncasset/Assets.h"

#include "ncutility/BinarySerialization.h"
#include "ncutility/NcError.h"
#include "fmt/format.h"

#include <istream>
#include <vector>

namespace
{
void ValidateHeader(const nc::asset::NcaHeader& header, std::string_view expectedMagicNumber)
{
    if (std::string_view{header.magicNumber} != expectedMagicNumber)
    {
        throw nc::NcError(fmt::format(
            "Magic number mismatch actual: '{}' expected: '{}' ",
             header.magicNumber, expectedMagicNumber)
        );
    }

    if (std::string_view{header.compressionAlgorithm} != "NONE")
    {
        throw nc::NcError(fmt::format(
            "Unsupported compression algorithm: '{}'",
            header.compressionAlgorithm
        ));
    }

    if (!nc::asset::IsVersionSupported(header.version))
    {
        throw nc::NcError(fmt::format(
            "Unsupported asset version: '{}'",
            header.version
        ));
    }
}

template<class T>
auto DeserializeImpl(std::istream& stream, std::string_view magicNumber) -> nc::asset::DeserializedResult<T>
{
    auto result = nc::asset::DeserializedResult<T>{};
    result.header = nc::asset::DeserializeHeader(stream);
    ::ValidateHeader(result.header, magicNumber);
    nc::serialize::Deserialize(stream, result.asset);
    return result;
}
} // anonymous namespace

namespace nc::asset
{
auto DeserializeHeader(std::istream& stream) -> NcaHeader
{
    auto header = nc::asset::NcaHeader{};
    nc::serialize::Deserialize(stream, header);
    return header;
}

auto DeserializeAudioClip(std::istream& stream) -> DeserializedResult<AudioClip>
{
    return DeserializeImpl<AudioClip>(stream, MagicNumber::audioClip);
}

auto DeserializeConcaveCollider(std::istream& stream) -> DeserializedResult<ConcaveCollider>
{
    return DeserializeImpl<ConcaveCollider>(stream, MagicNumber::concaveCollider);
}

auto DeserializeCubeMap(std::istream& stream) -> DeserializedResult<CubeMap>
{
    return DeserializeImpl<CubeMap>(stream, MagicNumber::cubeMap);
}

auto DeserializeHullCollider(std::istream& stream) -> DeserializedResult<HullCollider>
{
    return DeserializeImpl<HullCollider>(stream, MagicNumber::hullCollider);
}

auto DeserializeMesh(std::istream& stream) -> DeserializedResult<Mesh>
{
    return DeserializeImpl<Mesh>(stream, MagicNumber::mesh);
}

auto DeserializeSkeletalAnimation(std::istream& stream) -> DeserializedResult<SkeletalAnimation>
{
    return DeserializeImpl<SkeletalAnimation>(stream, MagicNumber::skeletalAnimation);
}

auto DeserializeTexture(std::istream& stream) -> DeserializedResult<Texture>
{
    return DeserializeImpl<Texture>(stream, MagicNumber::texture);
}
} // namespace nc::asset
