#pragma once

#include <iosfwd>

namespace nc
{
struct SceneFragment;

void Serialize(std::ostream& stream, const SceneFragment& in);
void Deserialize(std::istream& stream, SceneFragment& out);
} // namespace nc
