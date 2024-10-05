/**
 * @file Quaternion.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Vector.h"

namespace nc
{
/** @brief Quaternion type for representing 3D rotations. */
struct Quaternion
{
    float x, y, z, w;

    Quaternion()
        : x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
    {
    }

    Quaternion(float X, float Y, float Z, float W);

    Vector3 ToEulerAngles() const noexcept;
    void ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept;

    static Quaternion Identity() { return Quaternion{0.0f, 0.0f, 0.0f, 1.0f}; }
    static Quaternion FromEulerAngles(const Vector3& angles);
    static Quaternion FromEulerAngles(float x, float y, float z);
    static Quaternion FromAxisAngle(const Vector3& axis, float radians); // axis cannot be zero
};

/** @brief Return a normalized quaternion. */
Quaternion Normalize(const Quaternion& quat);

/** 
 * @brief Multiplies two quaternions.
 * @note For consistency with DirectXMath, the argument order is reversed from the order in which they are
 *       multiplied. In other words, this computes the lhs rotation followed by rhs (or the product rhs*lhs).
 */
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

/** @brief Finds rotation between lhs and rhs such that result * lhs == rhs. */
Quaternion Difference(const Quaternion& lhs, const Quaternion& rhs);

/** @brief Interpolates from lhs to rhs. */
Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float factor);

/** @brief Slerp from Identity to quat. */
Quaternion Scale(const Quaternion& quat, float factor);

inline bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
    return FloatEqual(lhs.x, rhs.x) &&
           FloatEqual(lhs.y, rhs.y) &&
           FloatEqual(lhs.z, rhs.z) &&
           FloatEqual(lhs.w, rhs.w);
}

inline bool operator!=(const Quaternion& lhs, const Quaternion& rhs)
{
    return !(lhs == rhs);
}
} // namespace nc
