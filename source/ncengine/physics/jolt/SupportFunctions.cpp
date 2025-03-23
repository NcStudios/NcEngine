#include "ncengine/physics/SupportFunctions.h"
#include "Conversion.h"
#include "CookedShapeUtility.h"

#include "ncmath/Vector.h"
#include "ncutility/platform/Platform.h"

#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CompoundShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace nc
{
struct SupportBuffer::Impl : public JPH::ConvexShape::SupportBuffer
{
};

SupportBuffer::SupportBuffer()
    : m_impl{std::make_unique<Impl>()}
{
}

SupportBuffer::SupportBuffer(SupportBuffer&&) noexcept = default;
SupportBuffer& SupportBuffer::operator=(SupportBuffer&&) noexcept = default;
SupportBuffer::~SupportBuffer() noexcept = default;

auto SupportBuffer::GetDefault() -> SupportBuffer&
{
    static auto instance = SupportBuffer{};
    return instance;
}
} // namespace nc

namespace
{
/** @note Support functions for non-compound shapes can ignore position/center of mass as its always zero. */

auto GetFurthestVertex(const JPH::Shape* shape,
                       const JPH::Vec3& direction,
                       nc::SupportBuffer& buffer) -> JPH::Vec3;

auto GetFurthestVertexLocal(const JPH::Shape* shape,
                            const JPH::Vec3& direction,
                            const JPH::Vec3& translation,
                            const JPH::Quat& rotation,
                            nc::SupportBuffer& buffer) -> JPH::Vec3;

[[noreturn]] NC_NO_INLINE
void UnhandledShapeType(JPH::EShapeType type,
                        JPH::EShapeSubType subtype)
{
    throw nc::NcError{fmt::format(
        "Unhandled shape type/subtype '{}', '{}'",
        std::to_underlying(type),
        std::to_underlying(subtype)
    )};
}

auto GetSupport(const JPH::SphereShape* sphere,
                const JPH::Vec3& direction) -> JPH::Vec3
{
    return direction * sphere->GetRadius();
}

auto GetSupport(const JPH::CapsuleShape* capsule,
                const JPH::Vec3& direction) -> JPH::Vec3
{
    const auto halfHeight = capsule->GetHalfHeightOfCylinder();
    const auto radius = capsule->GetRadius();
    const auto capsuleAxis = JPH::Vec3::sAxisY();
    const auto projection = std::clamp(direction.Dot(capsuleAxis), -1.0f, 1.0f);
    const auto furthestAxisPoint = projection * halfHeight * capsuleAxis;
    return furthestAxisPoint + direction * radius;
}

auto GetSupport(const JPH::ConvexShape* shape,
                const JPH::Vec3& direction,
                nc::SupportBuffer& buffer) -> JPH::Vec3
{
    const auto* supportFunc = shape->GetSupportFunction(
        JPH::ConvexShape::ESupportMode::Default,
        buffer.GetImpl(),
        JPH::Vec3::sOne()
    );

    return supportFunc->GetSupport(direction);
}

auto GetSupport(const JPH::ScaledShape* shape,
                const JPH::Vec3& direction,
                nc::SupportBuffer& buffer) -> JPH::Vec3
{
    const auto localVertex = GetFurthestVertex(shape, direction, buffer);
    return localVertex * shape->GetScale();
}

auto GetSupport(const JPH::RotatedTranslatedShape* shape,
                const JPH::Vec3& direction,
                nc::SupportBuffer& buffer) -> JPH::Vec3
{
    const auto& translation = shape->GetPosition();
    const auto& rotation = shape->GetRotation();
    return GetFurthestVertexLocal(shape->GetInnerShape(), direction, translation, rotation, buffer);
}

auto GetSupport(const JPH::CompoundShape* shape,
                const JPH::Vec3& direction,
                nc::SupportBuffer& buffer)
{
    const auto totalCenterOfMass = shape->GetCenterOfMass();
    auto furthestVertex = JPH::Vec3{};
    auto maxExtent = -1.0f;
    for (const auto& subShape : shape->GetSubShapes())
    {
        const auto translation = subShape.GetPositionCOM() + totalCenterOfMass;
        const auto rotation = subShape.GetRotation();
        const auto vertex = GetFurthestVertexLocal(subShape.mShape, direction, translation, rotation, buffer);
        const auto distance = vertex.Dot(direction);
        if (distance > maxExtent)
        {
            furthestVertex = vertex;
            maxExtent = distance;
        }
    }

    return furthestVertex;
}

auto GetFurthestVertexLocal(const JPH::Shape* shape,
                            const JPH::Vec3& direction,
                            const JPH::Vec3& translation,
                            const JPH::Quat& rotation,
                            nc::SupportBuffer& buffer) -> JPH::Vec3
{
    const auto localDirection = rotation.Conjugated() * direction;
    const auto localVertex = GetFurthestVertex(shape, localDirection, buffer);
    return rotation * localVertex + translation;
}

auto GetFurthestVertex(const JPH::Shape* shape,
                       const JPH::Vec3& direction,
                       nc::SupportBuffer& buffer) -> JPH::Vec3
{
    const auto type = shape->GetType();
    const auto subtype = shape->GetSubType();
    switch (type)
    {
        case JPH::EShapeType::Convex:
        {
            switch (subtype)
            {
                case JPH::EShapeSubType::Sphere:     return GetSupport(static_cast<const JPH::SphereShape*>(shape), direction);
                case JPH::EShapeSubType::Capsule:    return GetSupport(static_cast<const JPH::CapsuleShape*>(shape), direction);
                case JPH::EShapeSubType::Box:        [[fallthrough]];
                case JPH::EShapeSubType::ConvexHull: return GetSupport(static_cast<const JPH::ConvexShape*>(shape), direction, buffer);
                default:                             UnhandledShapeType(type, subtype);
            }
        }
        case JPH::EShapeType::Decorated:
        {
            switch (subtype)
            {
                case JPH::EShapeSubType::Scaled:            return GetSupport(static_cast<const JPH::ScaledShape*>(shape), direction, buffer);
                case JPH::EShapeSubType::RotatedTranslated: return GetSupport(static_cast<const JPH::RotatedTranslatedShape*>(shape), direction, buffer);
                default:                                    UnhandledShapeType(type, subtype);
            }
        }
        case JPH::EShapeType::Compound:
        {
            return GetSupport(static_cast<const JPH::CompoundShape*>(shape), direction, buffer);
        }
        default:
            UnhandledShapeType(type, subtype);
    }
}
} // anonymous namespace

namespace nc
{
auto GetWorldSupport(const CookedShape& shape,
                     const Vector3& directionNormal,
                     SupportBuffer& buffer) -> Vector3
{
    const auto& apiShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(apiShape.GetPtr(), direction, buffer);
    return physics::ToVector3(vertex);
}

auto GetHalfExtent(const CookedShape& shape,
                   const Vector3& directionNormal,
                   SupportBuffer& buffer) -> float
{
    const auto& apiShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(apiShape.GetPtr(), direction, buffer);

    if (HasIsometricTransformation(apiShape))
    {
        // note: For compound shapes, Shape::GetCenterOfMass() doesn't map to what the engine considers the origin.
        //       Instead, we can just shift back to origin IFF a translation is applied, ignoring COM offsets.
        const auto* decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(apiShape.GetPtr());
        return direction.Dot(vertex - decoratedShape->GetPosition());
    }

    return vertex.Dot(direction);
}
} // namespace nc
