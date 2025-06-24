#include "Serialize.h"
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
    constexpr auto dummySize = uint64_t{0}; // explicit size no longer needed, can reuse value in the future
    auto header = nc::asset::NcaHeader{"", "NONE", version, dummySize};
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

void Serialize(std::ostream& stream, const asset::MeshCollider& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::meshCollider, version);
}

void Serialize(std::ostream& stream, const asset::CubeMap& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::cubeMap, version);
}

void Serialize(std::ostream& stream, const asset::ConvexHull& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::convexHull, version);
}

void Serialize(std::ostream& stream, const asset::Mesh& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::mesh, version);
}

void Serialize(std::ostream& stream, const asset::ShapeKeyAnimation& data, uint64_t version)
{
    SerializeImpl(stream, data, asset::MagicNumber::shapeKeyAnimation, version);
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
