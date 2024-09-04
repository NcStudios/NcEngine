/**
 * @file Shape.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc::physics
{
/** @brief Minimum allowed scale for physics shapes. */
constexpr auto g_minimumShapeScale = 0.0001f;

/** @brief Options for Shape geometry. */
enum class ShapeType : uint8_t
{
    Box,
    Sphere,
    Capsule
};

/** @brief Get a valid scale for a shape given its current and desired scale values. */
auto NormalizeScaleForShape(nc::physics::ShapeType shape,
                            const Vector3& currentScale,
                            const Vector3& newScale) -> Vector3;

/** @brief Describes collision geometry for physics types. */
struct Shape
{
    /** @brief Make a primitive box shape. */
    static constexpr auto MakeBox(const Vector3& extents = Vector3::Splat(1.0f),
                                  const Vector3& localPosition = Vector3::Zero()) -> Shape
    {
        return Shape{localPosition, extents, ShapeType::Box};
    }

    /** @brief Make a primitive sphere shape. */
    static constexpr auto MakeSphere(float radius = 0.5f,
                                     const Vector3& localPosition = Vector3::Zero()) -> Shape
    {
        return Shape{localPosition, Vector3::Splat(radius * 2.0f), ShapeType::Sphere};
    }

    /** @brief Make a primitive capsule shape. */
    static constexpr auto MakeCapsule(float height = 2.0f,
                                      float radius = 0.5f,
                                      const Vector3& localPosition = Vector3::Zero()) -> Shape
    {
        return Shape{localPosition, Vector3{radius * 2.0f, height * 0.5f, radius * 2.0f}, ShapeType::Capsule};
    }

    auto GetLocalPosition() const -> const Vector3& { return m_localPosition; }
    auto GetLocalScale() const -> const Vector3& { return m_localScale; }
    auto GetType() const -> ShapeType { return m_type; }

    private:
        constexpr Shape(const Vector3& position, const Vector3& scale, ShapeType type)
            : m_localPosition{position}, m_localScale{scale}, m_type{type}
        {
        }

        Vector3 m_localPosition;
        Vector3 m_localScale;
        ShapeType m_type;
};
} // namespace nc::physics
