/**
 * @file Quaternion.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "Vector.h"

namespace nc
{
/** @brief Quaternion type for representing 3D rotations. */
struct Quaternion
{
    float x, y, z, w;

    constexpr explicit Quaternion()
        : x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
    {
    }

    explicit Quaternion(float X, float Y, float Z, float W);

    auto ToEulerAngles() const noexcept -> Vector3;
    void ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept;

    static constexpr auto Identity() -> Quaternion { return Quaternion{}; }
    static auto FromEulerAngles(const Vector3& angles) -> Quaternion;
    static auto FromEulerAngles(float x, float y, float z) -> Quaternion;
    static auto FromAxisAngle(const Vector3& axis, float radians) -> Quaternion; // axis cannot be zero
};

/** @brief Return a normalized quaternion. */
auto Normalize(const Quaternion& quat) -> Quaternion;

/** 
 * @brief Multiplies two quaternions.
 * @note For consistency with DirectXMath, the argument order is reversed from the order in which they are
 *       multiplied. In other words, this computes the lhs rotation followed by rhs (or the product rhs*lhs).
 */
auto Multiply(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion;

/** @brief Finds rotation between lhs and rhs such that result * lhs == rhs. */
auto Difference(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion;

/** @brief Interpolates from lhs to rhs. */
auto Slerp(const Quaternion& lhs, const Quaternion& rhs, float factor) -> Quaternion;

/** @brief Slerp from Identity to quat. */
auto Scale(const Quaternion& quat, float factor) -> Quaternion;

inline auto operator==(const Quaternion& lhs, const Quaternion& rhs) -> bool
{
    return FloatEqual(lhs.x, rhs.x) &&
           FloatEqual(lhs.y, rhs.y) &&
           FloatEqual(lhs.z, rhs.z) &&
           FloatEqual(lhs.w, rhs.w);
}

inline auto operator!=(const Quaternion& lhs, const Quaternion& rhs) -> bool
{
    return !(lhs == rhs);
}
} // namespace nc
