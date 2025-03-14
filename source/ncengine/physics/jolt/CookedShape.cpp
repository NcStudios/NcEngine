#include "ncengine/physics/CookedShape.h"
#include "Conversion.h"
#include "CookedShapeUtility.h"
#include "ShapeFactory.h"

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
} // namespace nc
