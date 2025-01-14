#include "ShapeFactory.h"
#include "Conversion.h"

#include "ncengine/asset/AssetData.h"
#include "ncjolt/ShapeUtility.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

#include <ranges>

namespace nc::physics
{
ShapeFactory::ShapeFactory(Signal<const asset::ConvexHullUpdateEventData&>& onConvexHullUpdate,
                           Signal<const asset::MeshColliderUpdateEventData&>& onMeshColliderUpdate)
    : m_convexHullUpdateConnection{onConvexHullUpdate.Connect(this, &ShapeFactory::OnConvexHullUpdate)},
      m_meshColliderUpdateConnection{onMeshColliderUpdate.Connect(this, &ShapeFactory::OnMeshColliderUpdate)}
{
}

auto ShapeFactory::MakeShape(const Shape& shape,
                             const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>
{
    const auto type = shape.GetType();
    const auto localPosition = ToJoltVec3(shape.GetLocalPosition());
    const auto localScale = ToJoltVec3(shape.GetLocalScale());
    const auto worldScale = localScale * additionalScaling;

    /** @todo: 694 support additional shape types */
    switch (type)
    {
        case ShapeType::Box:
            return MakeBox(worldScale * 0.5f, localPosition * additionalScaling);
        case ShapeType::Sphere:
            return MakeSphere(worldScale.GetX() * 0.5f, localPosition * additionalScaling);
        case ShapeType::Capsule:
            return MakeCapsule(worldScale.GetY() * 0.5f, worldScale.GetX() * 0.5f, localPosition * additionalScaling);
        case ShapeType::ConvexHull:
            return MakeConvexHull(shape.GetAssetId(), worldScale);
        case ShapeType::Mesh:
            return MakeMesh(shape.GetAssetId(), worldScale);
        default:
            NC_ASSERT(false, fmt::format("Unhandled ShapeType '{}'", std::to_underlying(type)));
            std::unreachable();
    };
}

auto ShapeFactory::MakeBox(const JPH::Vec3& halfExtents,
                           const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::BoxShape>(halfExtents, boxConvexRadius), localPosition);
}

auto ShapeFactory::MakeSphere(float radius,
                              const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::SphereShape>(radius), localPosition);
}

auto ShapeFactory::MakeCapsule(float halfHeight,
                               float radius,
                               const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return ApplyLocalOffsets(MakeRef<JPH::CapsuleShape>(halfHeight, radius), localPosition);
}

auto ShapeFactory::MakeConvexHull(asset::AssetId id,
                                  const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    NC_ASSERT(m_convexHulls.contains(id), "ConvexHull not loaded");
    return ApplyScale(m_convexHulls.at(id), scale);
}

auto ShapeFactory::MakeMesh(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    NC_ASSERT(m_meshColliders.contains(id), "MeshCollider not loaded");
    return ApplyScale(m_meshColliders.at(id), scale);
}

auto ShapeFactory::ApplyLocalOffsets(const JPH::Ref<JPH::Shape>& shape,
                                     const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::RotatedTranslatedShape>(localPosition, JPH::Quat::sIdentity(), shape.GetPtr());
}

auto ShapeFactory::ApplyScale(const JPH::Ref<JPH::Shape>& shape,
                              const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::ScaledShape>(shape.GetPtr(), scale);
}

void ShapeFactory::OnConvexHullUpdate(const asset::ConvexHullUpdateEventData& event)
{
    switch (event.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            for (const auto [asset, id] : std::views::zip(event.convexHulls, event.ids))
            {
                m_convexHulls.emplace(id, jolt::DeserializeShape(asset.blob));
            }

            break;
        }
        case asset::UpdateAction::Unload:
        {
            for (const auto id : event.ids)
            {
                m_convexHulls.erase(id);
            }

            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_convexHulls.clear();
            break;
        }
    }
}

void ShapeFactory::OnMeshColliderUpdate(const asset::MeshColliderUpdateEventData& event)
{
    switch (event.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            for (const auto [asset, id] : std::views::zip(event.colliders, event.ids))
            {
                m_meshColliders.emplace(id, jolt::DeserializeShape(asset.blob));
            }

            break;
        }
        case asset::UpdateAction::Unload:
        {
            for (const auto id : event.ids)
            {
                m_meshColliders.erase(id);
            }

            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_meshColliders.clear();
            break;
        }
    }
}
} // namespace nc::physics
