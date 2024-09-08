#include "JoltAssetLoader.h"
#include "Conversion.h"
#include "ShapeFactory.h"
#include "ncengine/asset/AssetData.h"

#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"

namespace nc::physics
{
JoltAssetLoader::JoltAssetLoader(ShapeFactory& factory)
    : m_shapeFactory{&factory}
{
}

auto JoltAssetLoader::ConvertToConvexHull(const asset::NamedMesh& mesh) const -> asset::ConvexHull
{
    auto points = JPH::Array<JPH::Vec3>{};
    points.reserve(mesh.vertices.size());
    for (const auto& vertex : mesh.vertices)
    {
        points.push_back(ToJoltVec3(vertex.position));
    }

    auto settings = JPH::ConvexHullShapeSettings{};
    settings.mPoints = std::move(points);
    settings.mMaxConvexRadius = JPH::cDefaultConvexRadius;
    const auto result = settings.Create();
    if (result.HasError())
    {
        throw NcError(fmt::format("Error creating convex hull: '{}'", result.GetError()));
    }

    const auto& shape = result.Get();

    /** @todo: #712 until hull asset manager is replaced, we need to sink here */
    m_shapeFactory->AddConvexHull(mesh.name, shape);

    return asset::ConvexHull{
        .shape = std::any{shape}
    };
}
} // namespace nc::physics
