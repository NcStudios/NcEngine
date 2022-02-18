#pragma once
#include "Math.h"

namespace nc
{
    struct Vector2
    {
        float x, y;

        constexpr explicit Vector2() noexcept : x{0.0f}, y{0.0f} {}
        constexpr explicit Vector2(float x, float y) noexcept : x{x}, y{y} {}
        static constexpr Vector2 Splat(float v) noexcept { return Vector2{v, v}; }
        static constexpr Vector2 Zero()  noexcept { return Vector2{ 0, 0}; }
        static constexpr Vector2 One()   noexcept { return Vector2{ 1, 1}; }
        static constexpr Vector2 Up()    noexcept { return Vector2{ 0, 1}; }
        static constexpr Vector2 Down()  noexcept { return Vector2{ 0,-1}; }
        static constexpr Vector2 Left()  noexcept { return Vector2{-1, 0}; }
        static constexpr Vector2 Right() noexcept { return Vector2{ 1, 0}; }
    };

    struct Vector3
    {
        float x, y, z;

        constexpr Vector3() noexcept : x{0.0f}, y{0.0f}, z{0.0f} {}
        constexpr explicit Vector3(float x, float y, float z) noexcept : x{x}, y{y}, z{z} {}
        constexpr explicit Vector3(const Vector2& v, float z) noexcept : x{v.x}, y{v.y}, z{z} {}
        static constexpr Vector3 Splat(float v) noexcept { return Vector3{v, v, v}; }
        static constexpr Vector3 Zero()  noexcept { return Vector3{ 0, 0, 0}; }
        static constexpr Vector3 One()   noexcept { return Vector3{ 1, 1, 1}; }
        static constexpr Vector3 Up()    noexcept { return Vector3{ 0, 1, 0}; }
        static constexpr Vector3 Down()  noexcept { return Vector3{ 0,-1, 0}; }
        static constexpr Vector3 Left()  noexcept { return Vector3{-1, 0, 0}; }
        static constexpr Vector3 Right() noexcept { return Vector3{ 1, 0, 0}; }
        static constexpr Vector3 Front() noexcept { return Vector3{ 0, 0, 1}; }
        static constexpr Vector3 Back()  noexcept { return Vector3{ 0, 0,-1}; }
    };

    struct Vector4
    {
        float x, y, z, w;

        constexpr explicit Vector4() noexcept : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {}
        constexpr explicit Vector4(float x, float y, float z, float w) noexcept : x{x}, y{y}, z{z}, w{w} {}
        constexpr explicit Vector4(const Vector3& v, float w) noexcept : x{v.x}, y{v.y}, z{v.z}, w{w} {}
        static constexpr Vector4 Splat(float v) noexcept { return Vector4{v, v, v, v}; }
        static constexpr Vector4 Zero() noexcept { return Vector4(0, 0, 0, 0); }
        static constexpr Vector4 One()  noexcept { return Vector4(1, 1, 1, 1); }
    };

    /* Vector2 Functions */
    constexpr auto operator ==(const Vector2& lhs, const Vector2& rhs) noexcept -> bool;
    constexpr auto operator !=(const Vector2& lhs, const Vector2& rhs) noexcept -> bool;
    constexpr auto operator +(const Vector2& lhs, const Vector2& rhs) noexcept -> Vector2;
    constexpr auto operator -(const Vector2& lhs, const Vector2& rhs) noexcept -> Vector2;
    constexpr auto operator *(const Vector2& vec, float scalar) noexcept -> Vector2;
    constexpr auto operator *(float scalar, const Vector2& vec) noexcept -> Vector2;
    constexpr auto operator /(const Vector2& vec, float scalar) noexcept -> Vector2;
    constexpr auto operator -(const Vector2& vec) noexcept -> Vector2;
    constexpr auto Dot(const Vector2& lhs, const Vector2& rhs) noexcept -> float;
    constexpr auto HadamardProduct(const Vector2& lhs, const Vector2& rhs) noexcept -> Vector2;
    constexpr auto SquareMagnitude(const Vector2& vec) noexcept -> float;
    inline    auto Magnitude(const Vector2& vec) noexcept -> float;
    inline    auto Normalize(const Vector2& vec) noexcept -> Vector2;
    constexpr auto Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept -> Vector2;
    constexpr auto SquareDistance(const Vector2& lhs, const Vector2& rhs) noexcept -> float;
    inline    auto Distance(const Vector2& lhs, const Vector2& rhs) noexcept -> float;
    constexpr auto HasAnyZeroElement(const Vector2& vec) noexcept -> bool;
    constexpr auto HasUniformElements(const Vector2& vec) noexcept -> bool;

    /* Vector3 Functions */
    constexpr auto operator ==(const Vector3& lhs, const Vector3& rhs) noexcept -> bool;
    constexpr auto operator !=(const Vector3& lhs, const Vector3& rhs) noexcept -> bool;
    constexpr auto operator +(const Vector3& lhs, const Vector3& rhs) noexcept -> Vector3;
    constexpr auto operator -(const Vector3& lhs, const Vector3& rhs) noexcept -> Vector3;
    constexpr auto operator *(const Vector3& vec, float scalar) noexcept -> Vector3;
    constexpr auto operator *(float scalar, const Vector3& vec) noexcept -> Vector3;
    constexpr auto operator /(const Vector3& vec, float scalar) noexcept -> Vector3;
    constexpr auto operator -(const Vector3& vec) noexcept -> Vector3;
    constexpr auto operator +=(Vector3& lhs, const Vector3& rhs) noexcept -> Vector3&;
    constexpr auto operator -=(Vector3& lhs, const Vector3& rhs) noexcept -> Vector3&;
    constexpr auto operator *=(Vector3& lhs, float rhs) noexcept -> Vector3&;
    constexpr auto operator /=(Vector3& lhs, float rhs) noexcept -> Vector3&;
    constexpr auto Dot(const Vector3& lhs, const Vector3& rhs) noexcept -> float;
    constexpr auto CrossProduct(const Vector3& lhs, const Vector3& rhs) noexcept -> Vector3;
    constexpr auto TripleCrossProduct(const Vector3& a, const Vector3& b, const Vector3& c) noexcept -> Vector3;
    constexpr auto HadamardProduct(const Vector3& lhs, const Vector3& rhs) noexcept -> Vector3;
    constexpr auto SquareMagnitude(const Vector3& vec) noexcept -> float;
    inline    auto Magnitude(const Vector3& vec) noexcept -> float;
    inline    auto Normalize(const Vector3& vec) noexcept -> Vector3;
    constexpr auto Lerp(const Vector3& lhs, const Vector3& rhs, float factor) noexcept -> Vector3;
    constexpr auto SquareDistance(const Vector3& lhs, const Vector3& rhs) noexcept -> float;
    inline    auto Distance(const Vector3& lhs, const Vector3& rhs) noexcept -> float;
    constexpr auto HasAnyZeroElement(const Vector3& vec) noexcept -> bool;
    constexpr auto HasUniformElements(const Vector3& vec) noexcept -> bool;
    inline    auto OrthogonalTo(const Vector3& vec) noexcept -> Vector3;
    inline    void OrthogonalBasis(const Vector3& vec, Vector3* a, Vector3* b) noexcept;

    /* Vector4 Functions */
    constexpr auto operator ==(const Vector4& lhs, const Vector4& rhs) noexcept -> bool;
    constexpr auto operator !=(const Vector4& lhs, const Vector4& rhs) noexcept -> bool;
    constexpr auto operator +(const Vector4& lhs, const Vector4& rhs) noexcept -> Vector4;
    constexpr auto operator -(const Vector4& lhs, const Vector4& rhs) noexcept -> Vector4;
    constexpr auto operator *(const Vector4& vec, float scalar) noexcept -> Vector4;
    constexpr auto operator *(float scalar, const Vector4& vec) noexcept -> Vector4;
    constexpr auto operator /(const Vector4& vec, float scalar) noexcept -> Vector4;
    constexpr auto operator -(const Vector4& vec) noexcept -> Vector4;
    constexpr auto Dot(const Vector4& lhs, const Vector4& rhs) noexcept -> float;
    constexpr auto HadamardProduct(const Vector4& lhs, const Vector4& rhs) noexcept -> Vector4;
    constexpr auto SquareMagnitude(const Vector4& vec) noexcept -> float;
    inline    auto Magnitude(const Vector4& vec) noexcept -> float;
    inline    auto Normalize(const Vector4& vec) noexcept -> Vector4;
    constexpr auto Lerp(const Vector4& lhs, const Vector4& rhs, float factor) noexcept -> Vector4;
    constexpr auto SquareDistance(const Vector4& lhs, const Vector4& rhs) noexcept -> float;
    inline    auto Distance(const Vector4& lhs, const Vector4& rhs) noexcept -> float;
    constexpr auto HasAnyZeroElement(const Vector4& vec) noexcept -> bool;
    constexpr auto HasUniformElements(const Vector4& vec) noexcept -> bool;
    constexpr auto ToVector3(const Vector4& vec) noexcept -> Vector3;

    /* Vector2 Implementation */
    constexpr bool operator ==(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y);
    }

    constexpr bool operator !=(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector2 operator +(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    constexpr Vector2 operator -(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    constexpr Vector2 operator *(const Vector2& vec, float scalar) noexcept
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }

    constexpr Vector2 operator *(float scalar, const Vector2& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector2 operator /(const Vector2& vec, float scalar) noexcept
    {
        return Vector2(vec.x / scalar, vec.y / scalar);
    }

    constexpr Vector2 operator -(const Vector2& vec) noexcept
    {
        return vec * -1.0f;
    }

    constexpr float Dot(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    constexpr Vector2 HadamardProduct(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2{lhs.x * rhs.x, lhs.y * rhs.y};
    }

    constexpr float SquareMagnitude(const Vector2& vec) noexcept
    {
        return Dot(vec, vec);
    }

    float Magnitude(const Vector2& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    Vector2 Normalize(const Vector2& vec) noexcept
    {
        return vec == Vector2::Zero() ? Vector2::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector2 Lerp(const Vector2& from, const Vector2& to, float factor) noexcept
    { 
        return math::Lerp(from, to, factor);
    }

    constexpr float SquareDistance(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return SquareMagnitude(lhs - rhs);
    }

    float Distance(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Magnitude(lhs - rhs);
    }

    constexpr bool HasAnyZeroElement(const Vector2& vec) noexcept
    {
        return math::FloatEqual(vec.x, 0.0f) || math::FloatEqual(vec.y, 0.0f);
    }

    constexpr auto HasUniformElements(const Vector2& vec) noexcept -> bool
    {
        return math::FloatEqual(vec.x, vec.y);
    }

    /* Vector3 Implementation */
    constexpr bool operator ==(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y) && math::FloatEqual(lhs.z, rhs.z);
    }

    constexpr bool operator !=(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector3 operator +(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    constexpr Vector3 operator -(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    constexpr Vector3 operator *(const Vector3& vec, float scalar) noexcept
    {
        return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar};
    }
    
    constexpr Vector3 operator *(float scalar, const Vector3& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector3 operator /(const Vector3& vec, float scalar) noexcept
    {
        return Vector3{vec.x / scalar, vec.y / scalar, vec.z / scalar};
    }

    constexpr Vector3 operator-(const Vector3& vec) noexcept
    {
        return vec * -1.0f;
    }

    constexpr Vector3& operator +=(Vector3& lhs, const Vector3& rhs) noexcept
    {
        lhs = lhs + rhs;
        return lhs;
    }

    constexpr Vector3& operator -=(Vector3& lhs, const Vector3& rhs) noexcept
    {
        lhs = lhs - rhs;
        return lhs;
    }

    constexpr Vector3& operator *=(Vector3& lhs, float rhs) noexcept
    {
        lhs = lhs * rhs;
        return lhs;
    }

    constexpr Vector3& operator /=(Vector3& lhs, float rhs) noexcept
    {
        lhs = lhs / rhs;
        return lhs;
    }

    constexpr float Dot(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
    
    constexpr Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3
        {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }

    constexpr Vector3 TripleCrossProduct(const Vector3& a, const Vector3& b, const Vector3& c) noexcept
    {
        return (b * Dot(c, a)) - (a * Dot(c, b));
    }

    constexpr Vector3 HadamardProduct(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
    }

    constexpr float SquareMagnitude(const Vector3& vec) noexcept
    {
        return Dot(vec, vec);
    }

    float Magnitude(const Vector3& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    Vector3 Normalize(const Vector3& vec) noexcept
    {
        return vec == Vector3::Zero() ? Vector3::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float factor) noexcept
    {
        return math::Lerp(lhs, rhs, factor);
    }

    constexpr float SquareDistance(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return SquareMagnitude(lhs - rhs);
    }

    float Distance(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Magnitude(lhs - rhs);
    }

    constexpr bool HasAnyZeroElement(const Vector3& vec) noexcept
    {
        return math::FloatEqual(vec.x, 0.0f) || math::FloatEqual(vec.y, 0.0f) || math::FloatEqual(vec.z, 0.0f);
    }

    constexpr auto HasUniformElements(const Vector3& vec) noexcept -> bool
    {
        return math::FloatEqual(vec.x, vec.y) && math::FloatEqual(vec.x, vec.z);
    }

    Vector3 OrthogonalTo(const Vector3& vec) noexcept
    {
        const float invSqrt3 = 1.0f / sqrt(3.0f);
        return (abs(vec.x) >= invSqrt3) ?
            Normalize(Vector3{vec.y, -1.0f * vec.x, 0.0f}) :
            Normalize(Vector3{0.0f, vec.z, -1.0f * vec.y});
    }

    void OrthogonalBasis(const Vector3& vec, Vector3* a, Vector3* b) noexcept
    {
        *a = OrthogonalTo(vec);
        *b = CrossProduct(*a, vec);
    }

    /* Vector 4 Implementation */
    constexpr bool operator ==(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) &&
               math::FloatEqual(lhs.y, rhs.y) &&
               math::FloatEqual(lhs.z, rhs.z) &&
               math::FloatEqual(lhs.w, rhs.w);
    }

    constexpr bool operator !=(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector4 operator +(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
    }

    constexpr Vector4 operator -(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
    }

    constexpr Vector4 operator *(const Vector4& vec, float scalar) noexcept
    {
        return Vector4{vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar};
    }

    constexpr Vector4 operator *(float scalar, const Vector4& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector4 operator /(const Vector4& vec, float scalar) noexcept
    {
        return Vector4{vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar};
    }

    constexpr Vector4 operator -(const Vector4& vec) noexcept
    {
        return vec * -1.0f;
    }

    constexpr float Dot(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }

    constexpr Vector4 HadamardProduct(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
    }

    constexpr float SquareMagnitude(const Vector4& vec) noexcept
    {
        return Dot(vec, vec);
    }

    float Magnitude(const Vector4& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    Vector4 Normalize(const Vector4& vec) noexcept
    {
        return vec == Vector4::Zero() ? Vector4::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector4 Lerp(const Vector4& lhs, const Vector4& rhs, float factor) noexcept
    {
        return math::Lerp(lhs, rhs, factor);
    }

    constexpr float SquareDistance(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return SquareMagnitude(lhs - rhs);
    }

    float Distance(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Magnitude(lhs - rhs);
    }

    constexpr bool HasAnyZeroElement(const Vector4& vec) noexcept
    {
        return math::FloatEqual(vec.x, 0.0f) ||
               math::FloatEqual(vec.y, 0.0f) ||
               math::FloatEqual(vec.z, 0.0f) ||
               math::FloatEqual(vec.w, 0.0f);
    }

    constexpr auto HasUniformElements(const Vector4& vec) noexcept -> bool
    {
        return math::FloatEqual(vec.x, vec.y) && math::FloatEqual(vec.x, vec.z) && math::FloatEqual(vec.x, vec.w);
    }

    constexpr Vector3 ToVector3(const Vector4& vec) noexcept
    {
        return Vector3{vec.x, vec.y, vec.z};
    }
} // namespace nc