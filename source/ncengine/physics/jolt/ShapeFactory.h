#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/utility/Signal.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"

#include <unordered_map>

namespace JPH
{
class Vec3;
} // namespace JPH

namespace nc
{
struct Shape;

namespace asset
{
struct ConvexHullUpdateEventData;
} // namespace asset

namespace physics
{
class ShapeFactory
{
    static constexpr auto boxConvexRadius = 0.025f;

    public:
        explicit ShapeFactory(Signal<const asset::ConvexHullUpdateEventData&>& onConvexHullUpdate);

        auto MakeShape(const Shape& shape, const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>;
        auto MakeBox(const JPH::Vec3& halfExtents, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeSphere(float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeCapsule(float halfHeight, float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeConvexHull(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>;

    private:
        std::unordered_map<asset::AssetId, JPH::Ref<JPH::Shape>> m_convexHulls;
        Connection m_convexHullUpdateConnection;

        template<class T, class... Args>
        auto MakeRef(Args&&... args) -> JPH::Ref<JPH::Shape>
        {
            return JPH::Ref<JPH::Shape>{new T(std::forward<Args>(args)...)};
        }

        auto ApplyLocalOffsets(const JPH::Ref<JPH::Shape>& shape,
                               const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;

        auto ApplyScale(const JPH::Ref<JPH::Shape>& shape,
                        const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>;

        void OnConvexHullUpdate(const asset::ConvexHullUpdateEventData& event);
};
} // namespace physics
} // namespace nc
