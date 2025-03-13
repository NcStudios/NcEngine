/**
 * @file Shape.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/physics/PhysicsLimits.h"
#include "ncmath/Vector.h"

namespace nc
{
/** @brief Options for Shape geometry. */
enum class ShapeType : uint8_t
{
    Box,
    Sphere,
    Capsule,
    ConvexHull,
    Mesh,
    Compound
};

/** @brief Get a valid scale for a shape given its current and desired scale values. */
auto NormalizeScaleForShape(nc::ShapeType shape,
                            const Vector3& currentScale,
                            const Vector3& newScale) -> Vector3;

/** @brief Describes collision geometry for physics types. */
struct Shape
{
    /** @brief Make a primitive box shape. */
    static constexpr auto MakeBox(const Vector3& extents = Vector3::Splat(1.0f)) -> Shape
    {
        return Shape{asset::NullAssetId, extents, ShapeType::Box};
    }

    /** @brief Make a primitive sphere shape. */
    static constexpr auto MakeSphere(float radius = 0.5f) -> Shape
    {
        return Shape{asset::NullAssetId, Vector3::Splat(radius * 2.0f), ShapeType::Sphere};
    }

    /** @brief Make a primitive capsule shape. */
    static constexpr auto MakeCapsule(float height = 2.0f,
                                      float radius = 0.5f) -> Shape
    {
        return Shape{asset::NullAssetId, Vector3{radius * 2.0f, height * 0.5f, radius * 2.0f}, ShapeType::Capsule};
    }

    /** @brief Make a shape from a ConvexHull asset. */
    static constexpr auto MakeConvexHull(asset::AssetId assetId,
                                         const Vector3& scale = Vector3::One()) -> Shape
    {
        return Shape{assetId, scale, ShapeType::ConvexHull};
    }

    /**
     * @brief Make a shape from a MeshCollider asset.
     * @note MeshColliders are only compatible with BodyType::Static.
     */
    static constexpr auto MakeMesh(asset::AssetId assetId,
                                   const Vector3& scale = Vector3::One()) -> Shape
    {
        return Shape{assetId, scale, ShapeType::Mesh};
    }

    /** @brief Make a shape from a CompoundShape asset. */
    static constexpr auto MakeCompound(asset::AssetId assetId,
                                       float scale = 1.0f) -> Shape
    {
        return Shape{assetId, Vector3::Splat(scale), ShapeType::Compound};
    }

    auto GetLocalScale()    const -> const Vector3& { return m_localScale; }
    auto GetAssetId()       const -> asset::AssetId { return m_assetId; }
    auto GetType()          const -> ShapeType      { return m_type; }

    private:
        constexpr Shape(asset::AssetId id, const Vector3& scale, ShapeType type)
            : m_assetId{id}, m_localScale{scale}, m_type{type}
        {
        }

        asset::AssetId m_assetId = asset::NullAssetId;
        Vector3 m_localScale = Vector3::One();
        ShapeType m_type;
};
} // namespace nc
