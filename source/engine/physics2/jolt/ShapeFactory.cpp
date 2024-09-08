#include "ShapeFactory.h"
#include "Conversion.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace nc::physics
{
auto ShapeFactory::MakeShape(const Shape& shape,
                             const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>
{
    const auto type = shape.GetType();
    const auto localPosition = ToJoltVec3(shape.GetLocalPosition());
    const auto localScale = ToJoltVec3(shape.GetLocalScale());
    const auto worldScale = localScale * additionalScaling;

    /** @todo: 693, 694 support additional shape types */
    switch (type)
    {
        case ShapeType::Box:
            return MakeBox(worldScale * 0.5f, localPosition * additionalScaling);
        case ShapeType::Sphere:
            return MakeSphere(worldScale.GetX() * 0.5f, localPosition * additionalScaling);
        case ShapeType::Capsule:
            return MakeCapsule(worldScale.GetY() * 0.5f, worldScale.GetX() * 0.5f, localPosition * additionalScaling);
        case ShapeType::ConvexHull:
            return MakeConvexHull(shape.GetAssetPath(), worldScale);
        default:
            NC_ASSERT(false, fmt::format("Unhandled ShapeType '{}'", std::to_underlying(type)));
            std::unreachable();
    };
}

auto ShapeFactory::MakeBox(const JPH::Vec3& halfExtents, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::BoxShape>(halfExtents, boxConvexRadius), localPosition);
}

auto ShapeFactory::MakeSphere(float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::SphereShape>(radius), localPosition);
}

auto ShapeFactory::MakeCapsule(float halfHeight, float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::CapsuleShape>(halfHeight, radius), localPosition);
}

auto ShapeFactory::MakeConvexHull(std::string_view asset, const JPH::Vec3& worldScale) -> JPH::Ref<JPH::Shape>
{
    (void)asset;
    (void)worldScale;
    /** @todo 712 fetch asset from manager */

    // todo: scale/pos ??
    return m_convexHulls.at(std::string{asset});
}

void ShapeFactory::AddConvexHull(std::string_view asset, JPH::Ref<JPH::Shape> shape)
{
    m_convexHulls.emplace(asset, std::move(shape));
}

auto ShapeFactory::ApplyLocalOffsets(const JPH::Ref<JPH::Shape>& shape,
                                     const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::RotatedTranslatedShape>(localPosition, JPH::Quat::sIdentity(), shape.GetPtr());
}
} // namespace nc::physics
