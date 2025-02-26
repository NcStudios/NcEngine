#pragma once

#include "ncengine/physics/CookedShape.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/Reference.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"

namespace nc
{
struct ShapeStorageRTTI
{
    using Raw_t = CookedShape::ShapeStorage;
    using Api_t = JPH::Ref<JPH::Shape>;

    static_assert(sizeof(Raw_t) == sizeof(Api_t));

    static void Construct(Raw_t& storage, const Api_t& shape)            { new (&storage.buffer) Api_t(shape); }
    static void Construct(Raw_t& storage, Api_t&& shape)                 { new (&storage.buffer) Api_t(std::move(shape)); }
    static void Destruct(Raw_t& storage)                                 { ToShape(storage).~Ref(); }
    static void Move(Raw_t& dst, Raw_t&& src)                            { ToShape(dst) = std::move(ToShape(src)); }
    static auto ToShape(Raw_t& storage)                  ->       Api_t& { return *reinterpret_cast<Api_t*>(&storage.buffer); }
    static auto ToShape(const Raw_t& storage)            -> const Api_t& { return *reinterpret_cast<const Api_t*>(&storage.buffer); }
};

inline auto HasIsometricTransformation(const JPH::Ref<JPH::Shape>& shape) -> bool
{
    return shape->GetSubType() == JPH::EShapeSubType::RotatedTranslated;
}

inline auto GetInnerShape(const JPH::Shape* shape, ShapeDecorationFlags::type& decorationsOut) -> const JPH::Shape*
{
    // Starting from the outer shape, nesting will always be:
    // RotatedTranslatedShape (optional)
    // ScaledShape (optional)
    // Actual Shape

    if (shape->GetSubType() == JPH::EShapeSubType::RotatedTranslated)
    {
        decorationsOut |= ShapeDecorationFlags::HasIsometricTransformation;
        shape = static_cast<const JPH::RotatedTranslatedShape*>(shape)->GetInnerShape();
    }

    if (shape->GetSubType() == JPH::EShapeSubType::Scaled)
    {
        decorationsOut |= ShapeDecorationFlags::HasScalingTransformation;
        shape = static_cast<const JPH::ScaledShape*>(shape)->GetInnerShape();
    }

    return shape;
}
} // namespace nc
