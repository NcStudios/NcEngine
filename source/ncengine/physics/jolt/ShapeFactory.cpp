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

namespace
{
template<class T, class... Args>
auto MakeRef(Args&&... args) -> JPH::Ref<JPH::Shape>
{
    return JPH::Ref<JPH::Shape>{new T(std::forward<Args>(args)...)};
}
} // anonymous namespace

namespace nc::physics
{
auto ShapeFactory::MakeDecoratedShape(const JPH::Shape* shape,
                                      const JPH::Vec3& localPosition,
                                      const JPH::Quat& localRotation) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::RotatedTranslatedShape>(localPosition, localRotation, shape);
}

auto ShapeFactory::MakeDecoratedShape(const JPH::Shape* shape,
                                      const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::ScaledShape>(shape, scale);
}

ShapeFactory::ShapeFactory(Signal<const asset::ConvexHullUpdateEventData&>& onConvexHullUpdate,
                           Signal<const asset::MeshColliderUpdateEventData&>& onMeshColliderUpdate)
    : m_convexHullUpdateConnection{onConvexHullUpdate.Connect(this, &ShapeFactory::OnConvexHullUpdate)},
      m_meshColliderUpdateConnection{onMeshColliderUpdate.Connect(this, &ShapeFactory::OnMeshColliderUpdate)}
{
    s_instance = this;
}

auto ShapeFactory::MakeShape(const Shape& shape,
                             const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>
{
    const auto type = shape.GetType();
    const auto localScale = ToJoltVec3(shape.GetLocalScale());
    const auto worldScale = localScale * additionalScaling;

    switch (type)
    {
        case ShapeType::Box:
            return MakeBox(worldScale * 0.5f);
        case ShapeType::Sphere:
            return MakeSphere(worldScale.GetX() * 0.5f);
        case ShapeType::Capsule:
            return MakeCapsule(worldScale.GetY(), worldScale.GetX() * 0.5f);
        case ShapeType::ConvexHull:
            return MakeConvexHull(shape.GetAssetId(), worldScale);
        case ShapeType::Mesh:
            return MakeMesh(shape.GetAssetId(), worldScale);
        case ShapeType::Compound:
            return MakeCompound(shape.GetAssetId(), worldScale);
        default:
            NC_ASSERT(false, fmt::format("Unhandled ShapeType '{}'", std::to_underlying(type)));
            std::unreachable();
    };
}

auto ShapeFactory::MakeShape(const Shape& shape,
                             const JPH::Vec3& position,
                             const JPH::Quat& rotation,
                             const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    auto innerShape = MakeShape(shape, scale);
    return ShapeFactory::MakeDecoratedShape(innerShape, position, rotation);
}

auto ShapeFactory::MakeBox(const JPH::Vec3& halfExtents) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::BoxShape>(halfExtents, boxConvexRadius);
}

auto ShapeFactory::MakeSphere(float radius) -> JPH::Ref<JPH::Shape>
{
    return MakeRef<JPH::SphereShape>(radius);
}

auto ShapeFactory::MakeCapsule(float halfHeight, float radius) -> JPH::Ref<JPH::Shape>
{
    const auto cylinderHalfHeight = std::max(halfHeight - radius, capsuleMinHalfHeight);
    return MakeRef<JPH::CapsuleShape>(cylinderHalfHeight, radius);
}

auto ShapeFactory::MakeConvexHull(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    return MakeDecoratedShape(GetConvexHull(id), scale);
}

auto ShapeFactory::MakeMesh(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    return MakeDecoratedShape(GetMeshCollider(id), scale);
}

auto ShapeFactory::MakeCompound(asset::AssetId id, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
{
    return MakeDecoratedShape(GetRuntimeAsset(id, ShapeType::Compound), scale);
}

auto ShapeFactory::GetConvexHull(asset::AssetId id) -> JPH::Shape*
{
    NC_ASSERT(m_convexHulls.contains(id), "ConvexHull not loaded");
    return m_convexHulls.at(id).GetPtr();
}

auto ShapeFactory::GetMeshCollider(asset::AssetId id) -> JPH::Shape*
{
    NC_ASSERT(m_meshColliders.contains(id), "MeshCollider not loaded");
    return m_meshColliders.at(id).GetPtr();
}

auto ShapeFactory::GetRuntimeAsset(asset::AssetId id, [[maybe_unused]] ShapeType type) -> JPH::Shape*
{
    NC_ASSERT(m_runtimeAssets.contains(id), fmt::format("Runtime asset '{}' is not loaded", id));
    auto& ref = m_runtimeAssets.at(id);
    NC_ASSERT(
        ToShapeType(ref->GetSubType()) == type,
        fmt::format(
            "Runtime asset type '{}' does not match expected '{}'",
            std::to_underlying(ToShapeType(ref->GetSubType())),
            std::to_underlying(type)
        )
    );

    return ref.GetPtr();
}

void ShapeFactory::AddRuntimeAsset(JPH::Ref<JPH::Shape> shape, asset::AssetId id)
{
    NC_ASSERT(!m_runtimeAssets.contains(id), fmt::format("AssetId '{}' already assigned to a runtime asset", id));
    m_runtimeAssets.emplace(id, std::move(shape));
}

void ShapeFactory::RemoveRuntimeAsset(asset::AssetId id)
{
    m_runtimeAssets.erase(id);
}

void ShapeFactory::RemoveAllRuntimeAssets()
{
    m_runtimeAssets.clear();
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
