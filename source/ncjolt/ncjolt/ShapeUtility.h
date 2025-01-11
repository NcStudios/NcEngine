#pragma once

#include <span>
#include <vector>

namespace JPH
{
template<class T>
class Ref;

class Shape;
} // namespace JPH

namespace nc
{
struct Vector3;

namespace jolt
{
/** @note Functions in this file require an instance of JoltApi to be initialized somewhere in the executable. */

// Build a ConvexHull Shape from a list of vertices.
auto BuildConvexHull(std::span<const Vector3> vertices) -> JPH::Ref<JPH::Shape>;

// Save a Shape to a binary blob.
auto SerializeShape(const JPH::Shape& shape) -> std::vector<uint8_t>;

// Load a shape from a binary blob.
auto DeserializeShape(std::vector<uint8_t> blob) -> JPH::Ref<JPH::Shape>;
} // namespace jolt
} // namespace nc
