/**
 * @file SupportFunctions.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

namespace nc
{
class CookedShape;
struct Vector3;

/** @brief Get the furthest vertex in a direction in world space. */
auto GetWorldSupport(const CookedShape& shape,
                     const Vector3& directionNormal) -> Vector3;

/** @brief Get the furthest vertex in a direction in local space. */
// auto GetLocalSupport(const CookedShape& shape,
//                      const Vector3& directionNormal) -> Vector3;

/** @brief  */
auto GetDistanceFromOrigin(const CookedShape& shape,
                           const Vector3& directionNormal) -> float;

/** @brief  */
auto GetHalfExtent(const CookedShape& shape,
                   const Vector3& directionNormal) -> float;
} // namespace nc
