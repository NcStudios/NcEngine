#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/scene/SceneFragment.h"

#include "ncutility/BinarySerialization.h"

#include <iostream>

namespace nc
{
void Serialize(std::ostream& stream, const SceneFragment& in)
{
    const auto header = SceneFragmentHeader{};
    serialize::Serialize(stream, header);
    serialize::Serialize(stream, in.assets);
    serialize::Serialize(stream, in.entities);
}

void Deserialize(std::istream& stream, SceneFragment& out)
{
    auto header = SceneFragmentHeader{};
    serialize::Deserialize(stream, header);
    if (header.magicNumber != g_sceneFragmentMagicNumber)
        throw NcError("Unexpected SceneFragment header");

    if (header.version != g_currentSceneFragmentVersion)
        throw NcError("Unexpected SceneFragment version");

    serialize::Deserialize(stream, out.assets);
    serialize::Deserialize(stream, out.entities);
}
} // namespace nc
