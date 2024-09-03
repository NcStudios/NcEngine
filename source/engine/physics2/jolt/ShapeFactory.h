#pragma once

#include "Conversion.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace nc::physics
{
inline auto NormalizeScaleForShape(nc::physics::ShapeType shape, JPH::Vec3& scaleOut) -> bool
{
    switch (shape)
    {
        case nc::physics::ShapeType::Box:
        {
            return false;
        }
        case nc::physics::ShapeType::Sphere:
        {
            if (JPH::ScaleHelpers::IsUniformScale(scaleOut))
            {
                return false;
            }

            scaleOut = JPH::ScaleHelpers::MakeUniformScale(scaleOut);
            return true;
        }
        default:
        {
            NC_ASSERT(false, fmt::format("Unhandled Shape: '{}'", std::to_underlying(shape)));
            std::unreachable();
        }
    }
}

class ShapeFactory
{
    static constexpr auto boxConvexRadius = 0.05f;

    public:
        auto MakeShape(const Shape& shape,
                        const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>
        {
            const auto type = shape.GetType();
            const auto localPosition = ToJoltVec3(shape.GetLocalPosition());
            const auto localScale = ToJoltVec3(shape.GetLocalScale());
            const auto worldScale = localScale * additionalScaling;

            /** @todo: 692, 693, 694 support additional shape types */
            switch (type)
            {
                case ShapeType::Box:
                    return MakeBox(worldScale * 0.5f, localPosition * additionalScaling);
                case ShapeType::Sphere:
                    NC_ASSERT(JPH::ScaleHelpers::IsUniformScale(worldScale), "Sphere requires uniform scale");
                    return MakeSphere(worldScale.GetX() * 0.5f, localPosition * additionalScaling);
                default:
                    NC_ASSERT(false, fmt::format("Unhandled ShapeType '{}'", std::to_underlying(type)));
                    std::unreachable();
            };
        }

        auto MakeBox(const JPH::Vec3& halfExtents, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
        {
            return ApplyLocalOffsets(MakeRef<JPH::BoxShape>(halfExtents, boxConvexRadius), localPosition);
        }

        auto MakeSphere(float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
        {
            return ApplyLocalOffsets(MakeRef<JPH::SphereShape>(radius), localPosition);
        }

    private:
        template<class T, class... Args>
        auto MakeRef(Args&&... args) -> JPH::Ref<JPH::Shape>
        {
            return JPH::Ref<JPH::Shape>{new T(std::forward<Args>(args)...)};
        }

        auto ApplyLocalOffsets(const JPH::Ref<JPH::Shape>& shape,
                               const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
        {
            return MakeRef<JPH::RotatedTranslatedShape>(localPosition, JPH::Quat::sIdentity(), shape.GetPtr());
        }
};
} // namespace nc::physics
