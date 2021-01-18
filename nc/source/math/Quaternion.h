#pragma once

#include "Vector3.h"

namespace DirectX { struct XMFLOAT4; }

namespace nc
{
    struct AxisAngle
    {
        Vector3 axis;
        float angle;
    };

    struct Quaternion
    {
        float x, y, z, w;

        Quaternion(float X, float Y, float Z, float W) noexcept
            : x{X}, y{Y}, z{Z}, w{W} {}

        Quaternion(const DirectX::XMFLOAT4& xmf4) noexcept;
        Quaternion(DirectX::XMFLOAT4&& xmf4) noexcept;

        AxisAngle ToAxisAngle() const noexcept;

        static Quaternion Identity() { return Quaternion{0.0f, 0.0f, 0.0f, 1.0f}; }
        static Quaternion Zero() { return Quaternion{0.0f, 0.0f, 0.0f, 0.0f}; } //invalid state, only use for error checking
        static Quaternion FromEulerAngles(const Vector3& angles);
        static Quaternion FromEulerAngles(float x, float y, float z);
    };

    bool operator==(const Quaternion& lhs, const Quaternion& rhs);
}