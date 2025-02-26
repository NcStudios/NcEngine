/**
 * @file CookedShape.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/physics/Shape.h"
#include "ncmath/Quaternion.h"

namespace nc
{
/** @brief Flags describing internal structure of a CookedShape. */
struct ShapeDecorationFlags
{
    using type = uint8_t;

    static constexpr type None                       = 0;      // shape is exactly what ShapeType
    static constexpr type HasScalingTransformation   = 1 << 0; // shape has additional scaling; never set for primitives
    static constexpr type HasIsometricTransformation = 1 << 1; // shape has position/rotation offsets
    static constexpr type IsStaticCompound           = 1 << 2; // shape was cooked as a static compound
};

/** @brief Properties of a CookedShape. */
struct CookedShapeProperties
{
    ShapeType type = ShapeType::Box;
    ShapeDecorationFlags::type decorations = ShapeDecorationFlags::None;
};

/** @brief Handle to a processed Shape instance. */
class CookedShape
{
    public:
        struct ShapeStorage
        {
            unsigned char buffer[8];
        };

        /** @brief Cook from a Shape. */
        CookedShape(const Shape& shape);

        /** @brief Cook from a Shape, applying additional transformations. */
        CookedShape(const Shape& shape, const Vector3& position, const Quaternion& rotation);

        /** @brief Construct from existing cooked data. */
        CookedShape(const ShapeStorage& cookedData);

        CookedShape(CookedShape&& other) noexcept;
        CookedShape& operator=(CookedShape&& other) noexcept;
        CookedShape(const CookedShape&) = delete;
        CookedShape& operator=(const CookedShape&) = delete;
        ~CookedShape() noexcept;

        /** @brief Check if the internal shape is still valid (e.g. hasn't been moved from). */
        auto HasShape() const noexcept -> bool;

        /** @brief Get the type and decorations of the CookedShape. */
        auto GetProperties() const -> CookedShapeProperties;

        /** @brief Get the shape's translation (will be the origin, unless the shape was cooked with a position). */
        auto GetPosition() const -> Vector3;

        /** @brief Get the shape's rotation (will be identity, unless the shape was cooked with a rotation). */
        auto GetRotation() const -> Quaternion;

        /** @brief Partially recook the shape with a new position and rotation. */
        void SetPositionAndRotation(const Vector3& position, const Quaternion& rotation);

        /** @cond internal */
        auto GetShapeData() const -> const ShapeStorage& { return m_storage; }
        /** @endcond */

    private:
        ShapeStorage m_storage;
};
} // namespace nc
