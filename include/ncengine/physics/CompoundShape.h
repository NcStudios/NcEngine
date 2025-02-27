/**
 * @file ShapeBuilder.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/physics/Shape.h"
#include "ncengine/physics/CookedShape.h"
#include "ncmath/Quaternion.h"

#include <memory>

namespace nc
{
/** @brief Index of a SubShape within a CompoundShape. */
using SubShapeIndex = uint32_t;

/** @brief Describes a SubShape to be added to a compound shape. */
struct SubShapeInfo
{
    Shape shape = Shape::MakeBox();               ///< subshape info; supported types depend on mutable/static build option
    Vector3 position = Vector3::Zero();           ///< local position of the subshape
    Quaternion rotation = Quaternion::Identity(); ///< local rotation of the subshape
    uint32_t userData = 0;                        ///< optional data for the subshape
};

/**
 * @brief Construct a CompoundShape that can be modifed after cooking.
 * @note Does not support adding other compound shapes as subshapes.
 */
auto CreateMutableCompoundShape(std::span<const SubShapeInfo> shapes) -> CookedShape;

/**
 * @brief Construct a CompoundShape that cannot be modified after cooking.
 * @brief Does not support adding other compound shapes or meshes as subshapes.
 */
auto CreateStaticCompoundShape(std::span<const SubShapeInfo> shapes) -> CookedShape;

/**
 * @brief Interface for modifying a mutable CompoundShape.
 * 
 * Mutable CompoundShapes trade some query performance for the ability to be modified after cooking. The builder
 * applies modifications directly to the provided CookedShape, holding a strong reference to it. Modification is
 * only supported for untransformed shapes (cooking must be done with identity position, rotation, and scale).
 */
class CompoundShapeBuilder
{
    public:
        /** @brief Construct a builder from a cooked mutable CompoundShape. */
        explicit CompoundShapeBuilder(CookedShape& shape);
        ~CompoundShapeBuilder() noexcept;

        /** @brief Get the number of SubShapes. */
        auto GetSubShapeCount() const -> uint32_t;

        /**
         * @brief Get the index of the SubShape with userData.
         * @note Requires each SubShape to have unique user data.
         */
        auto GetSubShapeIndex(uint32_t userData) const -> SubShapeIndex;

        /** @brief Get the local position of a SubShape. */
        auto GetSubShapePosition(SubShapeIndex index) const -> Vector3;

        /** @brief Get the local rotation of a SubShape. */
        auto GetSubShapeRotation(SubShapeIndex index) const -> Quaternion;

        /** @brief Add a new SubShape. */
        auto AddSubShape(const SubShapeInfo& info) -> SubShapeIndex;

        /** @brief Remove the SubShape at index. */
        void RemoveSubShape(SubShapeIndex index);

        /** @brief Update the position and rotation of a SubShape. */
        void ModifySubShape(SubShapeIndex index,
                            const Vector3& position,
                            const Quaternion& rotation);

        /** @brief Replace the SubShape matching shapeId (user data will be unchanged). */
        void ReplaceSubShape(SubShapeIndex index,
                             const SubShapeInfo& info);

        /** @brief Update internal COM offsets, if the shape has been modified significantly. */
        void RecalculateCenterOfMass();

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
};
} // namespace nc
