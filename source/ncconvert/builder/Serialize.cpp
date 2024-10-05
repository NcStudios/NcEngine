#include "Serialize.h"
#include "utility/BlobSize.h"
#include "ncasset/Assets.h"
#include "ncasset/NcaHeader.h"

#include "ncutility/BinarySerialization.h"

#include <cstring>
#include <iostream>

namespace
{
template<class T>
void SerializeImpl(std::ostream& stream, const T& data, std::string_view magicNumber, uint64_t version)
{
    auto header = nc::asset::NcaHeader{"", "NONE", version, nc::convert::GetBlobSize(data)};
    std::memcpy(header.magicNumber, magicNumber.data(), 5);
    nc::serialize::Serialize(stream, header);
    nc::serialize::Serialize(stream, data);
}
} // anonymous namespace

namespace nc::convert
{
void Serialize(std::ostream& stream, const asset::AudioClip& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::audioClip, version);
}

void Serialize(std::ostream& stream, const asset::ConcaveCollider& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::concaveCollider, version);
}

void Serialize(std::ostream& stream, const asset::CubeMap& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::cubeMap, version);
}

void Serialize(std::ostream& stream, const asset::HullCollider& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::hullCollider, version);
}

void Serialize(std::ostream& stream, const asset::Mesh& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::mesh, version);
}

void Serialize(std::ostream& stream, const asset::SkeletalAnimation& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::skeletalAnimation, version);
}

void Serialize(std::ostream& stream, const asset::Texture& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::texture, version);
}
} // namespace nc::convert
