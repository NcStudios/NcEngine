#include "ncjolt/ShapeUtility.h"
#include "ncjolt/ByteArrayStream.h"

#include "ncmath/Vector.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"

#include <ranges>

namespace nc::jolt
{
auto BuildConvexHull(std::span<const Vector3> vertices) -> JPH::Ref<JPH::Shape>
{
    auto convertedVertices = JPH::Array<JPH::Vec3>{};
    convertedVertices.reserve(vertices.size());
    std::ranges::transform(
        vertices,
        std::back_inserter(convertedVertices),
        [](const auto& in) { return JPH::Vec3{reinterpret_cast<const JPH::Float3&>(in)}; }
    );

    auto settings = JPH::ConvexHullShapeSettings{};
    settings.mPoints = std::move(convertedVertices);
    settings.mMaxConvexRadius = JPH::cDefaultConvexRadius;
    const auto result = settings.Create();
    if (result.HasError())
    {
        throw NcError(fmt::format("Error creating convex hull: '{}'", result.GetError()));
    }

    return result.Get();
}

auto SerializeShape(const JPH::Shape& shape) -> std::vector<uint8_t>
{
    auto stream = ByteArrayStream{};
    shape.SaveBinaryState(stream);
    return stream.ExtractBuffer();
}

auto DeserializeShape(std::vector<uint8_t> blob) -> JPH::Ref<JPH::Shape>
{
    auto stream = ByteArrayStream{std::move(blob)};
    auto result = JPH::Shape::sRestoreFromBinaryState(stream);
    if (result.HasError())
    {
        throw NcError(fmt::format("Error reading binary data for shape: '{}'", result.GetError()));
    }

    return result.Get();
}
} // namespace nc::jolt
