#include "ncjolt/ShapeUtility.h"
#include "ncjolt/ByteArrayStream.h"

#include "ncmath/Geometry.h"
#include "ncmath/Vector.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"

#include <ranges>

namespace
{
auto ToFloat3(const nc::Vector3& in) -> const JPH::Float3&
{
    return reinterpret_cast<const JPH::Float3&>(in);
}

auto ToJoltTriangle(const nc::Triangle& in) -> JPH::Triangle
{
    return JPH::Triangle{ToFloat3(in.a), ToFloat3(in.b), ToFloat3(in.c)};
}
} // anonymous namespace

namespace nc::jolt
{
auto BuildConvexHull(std::span<const Vector3> vertices) -> JPH::Ref<JPH::Shape>
{
    const auto convertedVertices = vertices
        | std::views::transform([](const auto& in) { return JPH::Vec3{ToFloat3(in)}; })
        | std::ranges::to<JPH::Array<JPH::Vec3>>();

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

auto BuildMeshShape(std::span<const Triangle> triangles) -> JPH::Ref<JPH::Shape>
{
    const auto convertedTriangles = triangles
        | std::views::transform(ToJoltTriangle)
        | std::ranges::to<JPH::Array<JPH::Triangle>>();

    const auto settings = JPH::MeshShapeSettings{convertedTriangles};
    const auto result = settings.Create();
    if (result.HasError())
    {
        throw NcError{fmt::format("Error creating mesh shape: '{}'", result.GetError())};
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
