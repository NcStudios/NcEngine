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
struct MeshColliderUpdateEventData;
} // namespace asset

namespace physics
{
class ShapeFactory
{
    static constexpr auto boxConvexRadius = 0.025f;

    public:
        explicit ShapeFactory(Signal<const asset::ConvexHullUpdateEventData&>& onConvexHullUpdate,
                              Signal<const asset::MeshColliderUpdateEventData&>& onMeshColliderUpdate);

        static auto Instance() -> ShapeFactory* { return s_instance; }

        static auto MakeDecoratedShape(const JPH::Shape* shape,
                                       const JPH::Vec3& localPosition,
                                       const JPH::Quat& localRotation) -> JPH::Ref<JPH::Shape>;

        static auto MakeDecoratedShape(const JPH::Shape* shape,
                                       const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>;

        auto MakeShape(const Shape& shape,
                       const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>;

        auto MakeShape(const Shape& shape,
                       const JPH::Vec3& position,
                       const JPH::Quat& rotation,
                       const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>;

        auto MakeBox(const JPH::Vec3& halfExtents)                     -> JPH::Ref<JPH::Shape>;
        auto MakeSphere(float radius)                                  -> JPH::Ref<JPH::Shape>;
        auto MakeCapsule(float halfHeight, float radius)               -> JPH::Ref<JPH::Shape>;
        auto MakeConvexHull(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>;
        auto MakeMesh(asset::AssetId id, const JPH::Vec3& scale)       -> JPH::Ref<JPH::Shape>;
        auto MakeCompound(asset::AssetId id, const JPH::Vec3& scale)   -> JPH::Ref<JPH::Shape>;
        auto GetConvexHull(asset::AssetId id)                          -> JPH::Shape*           { return m_convexHulls.at(id).GetPtr(); }
        auto GetMeshCollider(asset::AssetId id)                        -> JPH::Shape*           { return m_meshColliders.at(id).GetPtr(); }

    private:
        inline static ShapeFactory* s_instance = nullptr;

        std::unordered_map<asset::AssetId, JPH::Ref<JPH::Shape>> m_convexHulls;
        std::unordered_map<asset::AssetId, JPH::Ref<JPH::Shape>> m_meshColliders;
        Connection m_convexHullUpdateConnection;
        Connection m_meshColliderUpdateConnection;

        void OnConvexHullUpdate(const asset::ConvexHullUpdateEventData& event);
        void OnMeshColliderUpdate(const asset::MeshColliderUpdateEventData& event);
};
} // namespace physics
} // namespace nc
