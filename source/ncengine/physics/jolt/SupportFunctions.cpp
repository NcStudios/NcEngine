#include "ncengine/physics/SupportFunctions.h"
#include "Conversion.h"
#include "CookedShapeUtility.h"

#include "ncmath/Vector.h"

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
    auto supportFunc = shape->GetSupportFunction(
        JPH::ConvexShape::ESupportMode::Default,
        buffer,
        JPH::Vec3::sOne()
    );

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
auto GetWorldSupport(const CookedShape& shape,
                     const Vector3& directionNormal) -> Vector3
{
    const auto& apiShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto point = ::GetFurthestVertex(apiShape.GetPtr(), direction);
    return physics::ToVector3(point);
}

auto GetDistanceFromOrigin(const CookedShape& shape,
                           const Vector3& directionNormal) -> float
{
    const auto& apiShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(apiShape.GetPtr(), direction);
    return vertex.Dot(direction);
}

auto GetHalfExtent(const CookedShape& shape,
                   const Vector3& directionNormal) -> float
{
    const auto& apiShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    const auto direction = physics::ToJoltVec3(directionNormal);
    const auto vertex = ::GetFurthestVertex(apiShape.GetPtr(), direction);

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
