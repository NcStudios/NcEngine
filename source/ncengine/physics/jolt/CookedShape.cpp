#include "ncengine/physics/CookedShape.h"
#include "Conversion.h"
#include "CookedShapeUtility.h"
#include "ShapeFactory.h"

#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CompoundShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace
{
auto GetSupport(const JPH::SphereShape* sphere,
                const JPH::Vec3& direction) -> JPH::Vec3
{
    return sphere->GetCenterOfMass() + direction * sphere->GetRadius();
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
                const JPH::Vec3& direction) -> JPH::Vec3
{
    // todo: not thread safe
    static auto buffer = JPH::ConvexShape::SupportBuffer{};
    auto supportFunc = shape->GetSupportFunction(JPH::ConvexShape::ESupportMode::Default, buffer, JPH::Vec3::sOne());
    return supportFunc->GetSupport(direction);
}

auto GetFurthestVertex(const JPH::Shape* shape,
                       const JPH::Vec3& direction) -> JPH::Vec3;

auto GetFurthestVertexLocal(const JPH::Shape* shape,
                            const JPH::Vec3& direction,
                            const JPH::Vec3& translation,
                            const JPH::Quat& rotation) -> JPH::Vec3
{
    const auto localDirection = rotation.Conjugated() * direction;
    const auto localVertex = GetFurthestVertex(shape, localDirection);
    return rotation * localVertex + translation;
}

auto GetFurthestVertex(const JPH::Shape* shape,
                       const JPH::Vec3& direction) -> JPH::Vec3
{
    switch (shape->GetSubType())
    {
        case JPH::EShapeSubType::Sphere:
        {
            const auto* sphereShape = static_cast<const JPH::SphereShape*>(shape);
            return GetSupport(sphereShape, direction);
        }
        case JPH::EShapeSubType::Capsule:
        {
            const auto* capsuleShape = static_cast<const JPH::CapsuleShape*>(shape);
            return GetSupport(capsuleShape, direction);
        }
    }

    switch (shape->GetType())
    {
        case JPH::EShapeType::Convex:
        {
            const auto* convexShape = static_cast<const JPH::ConvexShape*>(shape);
            return GetSupport(convexShape, direction);
        }
        case JPH::EShapeType::Decorated:
        {
            switch (shape->GetSubType())
            {
                case JPH::EShapeSubType::Scaled:
                {
                    const auto* decoratedShape = static_cast<const JPH::ScaledShape*>(shape);
                    const auto localVertex = GetFurthestVertex(decoratedShape->GetInnerShape(), direction);
                    return localVertex * decoratedShape->GetScale();
                }
                case JPH::EShapeSubType::RotatedTranslated:
                {
                    const auto* decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(shape);
                    const auto& translation = decoratedShape->GetPosition();
                    const auto& rotation = decoratedShape->GetRotation();
                    return GetFurthestVertexLocal(decoratedShape->GetInnerShape(), direction, translation, rotation);
                }
                default:
                    throw nc::NcError{fmt::format("Unhandled SubShapeType '{}'", (int)shape->GetSubType())};
            }
        }
        case JPH::EShapeType::Compound:
        {
            const auto* compoundShape = static_cast<const JPH::CompoundShape*>(shape);
            const auto totalCenterOfMass = compoundShape->GetCenterOfMass();
            auto furthestVertex = JPH::Vec3{};
            auto maxExtent = -FLT_MAX;
            for (const auto& subShape : compoundShape->GetSubShapes())
            {
                const auto translation = subShape.GetPositionCOM() + totalCenterOfMass;
                const auto rotation = subShape.GetRotation();
                const auto vertex = GetFurthestVertexLocal(subShape.mShape, direction, translation, rotation);
                const auto distance = vertex.Dot(direction);
                if (distance > maxExtent)
                {
                    furthestVertex = vertex;
                    maxExtent = distance;
                }
            }

            return furthestVertex;
        }

        default:
            throw nc::NcError{fmt::format("Unhandled ShapeType '{}'", (int)shape->GetType())};
        }
    }
} // anonymous namespace

namespace nc
{
CookedShape::CookedShape(const Shape& shape)
{
    ShapeStorageRTTI::Construct(
        m_storage,
        physics::ShapeFactory::Instance()->MakeShape(
            shape,
            JPH::Vec3::sOne()
        )
    );
}

CookedShape::CookedShape(const Shape& shape,
                         const Vector3& position,
                         const Quaternion& rotation)
{
    ShapeStorageRTTI::Construct(
        m_storage,
        physics::ShapeFactory::Instance()->MakeShape(
            shape,
            physics::ToJoltVec3(position),
            physics::ToJoltQuaternion(rotation),
            JPH::Vec3::sOne()
        )
    );
}

CookedShape::CookedShape(const ShapeStorage& cookedData)
{
    ShapeStorageRTTI::Construct(m_storage, ShapeStorageRTTI::ToShape(cookedData));
}

CookedShape::CookedShape(CookedShape&& other) noexcept
{
    ShapeStorageRTTI::Construct(m_storage, std::move(ShapeStorageRTTI::ToShape(other.m_storage)));
}

CookedShape& CookedShape::operator=(CookedShape&& other) noexcept
{
    if (this != &other)
    {
        ShapeStorageRTTI::Move(m_storage, std::move(other.m_storage));
    }

    return *this;
}

CookedShape::~CookedShape() noexcept
{
    ShapeStorageRTTI::Destruct(m_storage);
}

auto CookedShape::HasShape() const noexcept -> bool
{
    return ShapeStorageRTTI::ToShape(m_storage).GetPtr() != nullptr;
}

auto CookedShape::GetProperties() const -> CookedShapeProperties
{
    auto properties = CookedShapeProperties{};
    const auto& outerShape = ShapeStorageRTTI::ToShape(m_storage);
    const auto innerShape = GetInnerShape(outerShape.GetPtr(), properties.decorations);
    properties.type = physics::ToShapeType(innerShape->GetSubType());
    if (innerShape->GetSubType() == JPH::EShapeSubType::StaticCompound)
    {
        properties.decorations |= ShapeDecorationFlags::IsStaticCompound;
    }

    return properties;
}

auto CookedShape::GetPosition() const -> Vector3
{
    const auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    return physics::ToVector3(shape->GetCenterOfMass());
}

auto CookedShape::GetRotation() const -> Quaternion
{
    const auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    if (HasIsometricTransformation(shape))
    {
        const auto& decorated = static_cast<const JPH::RotatedTranslatedShape*>(shape.GetPtr());
        return physics::ToQuaternion(decorated->GetRotation());
    }

    return Quaternion::Identity();
}

void CookedShape::SetPositionAndRotation(const Vector3& position, const Quaternion& rotation)
{
    auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    const JPH::Shape* innerShape = shape.GetPtr();
    if (HasIsometricTransformation(shape))
    {
        auto decorated = static_cast<JPH::RotatedTranslatedShape*>(shape.GetPtr());
        innerShape = decorated->GetInnerShape();
    }

    shape = physics::ShapeFactory::MakeDecoratedShape(
        innerShape,
        physics::ToJoltVec3(position),
        physics::ToJoltQuaternion(rotation)
    );
}

auto CookedShape::GetFurthestVertex(const Vector3& directionNormal) const -> Vector3
{
    const auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto point = ::GetFurthestVertex(shape.GetPtr(), direction);
    return physics::ToVector3(point);
}

auto CookedShape::GetDistanceFromOrigin(const Vector3& directionNormal) const -> float
{
    const auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(shape.GetPtr(), direction);
    return vertex.Dot(direction);
}

auto CookedShape::GetHalfExtent(const Vector3& directionNormal) const -> float
{
    const auto& shape = ShapeStorageRTTI::ToShape(m_storage);
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(shape.GetPtr(), direction);
    return direction.Dot(vertex - shape->GetCenterOfMass());
}
} // namespace nc
