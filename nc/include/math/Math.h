#pragma once

#include <cstdint>
#include <concepts>
#include <cmath>
#include <limits>
#include <numbers>

namespace nc::math
{
    constexpr auto FloatEpsilon = std::numeric_limits<float>::epsilon();
    constexpr auto DoubleEpsilon = std::numeric_limits<double>::epsilon();

    template<std::totally_ordered T>
    constexpr T Min(T a, T b) noexcept { return a < b ? a : b; }
    
    template<std::totally_ordered T>
    constexpr T Max(T a, T b) noexcept { return a > b ? a : b; }

    template<std::floating_point T>
    constexpr T Ceiling(T val) noexcept { return (int)(val + 1); }
    
    template<std::floating_point T>
    constexpr T Floor (T val) noexcept { return (int) val; }
    
    template<std::floating_point T>
    constexpr T Round (T val) noexcept { return (int)(val + 0.5); }

    template<std::totally_ordered T>
    constexpr T Clamp(T val, T min, T max) noexcept
    {
        if(val < min)
            val = min;
        else if(val > max)
            val = max;
        return val;
    }

    template<std::floating_point T>
    constexpr T Lerp(T a, T b, T factor) noexcept { return a + (b - a) * factor; }

    template<std::floating_point T>
    constexpr T WrapAngle(T theta) noexcept
    {
        const T modTwoPi = std::fmod(theta, 2.0L * std::numbers::pi);
        return (modTwoPi > std::numbers::pi ? (modTwoPi - 2.0L * std::numbers::pi) : modTwoPi);
    }

    template<std::floating_point T>
    constexpr T DegreesToRadians(T degrees) noexcept { return degrees * std::numbers::pi / 180.0; }
    
    template<std::floating_point T>
    constexpr T RadiansToDegrees(T radians) noexcept { return radians * 180.0 / std::numbers::pi; }

    constexpr bool FloatEqual(float a, float b, float maxAbsDiff = 0.00001f, float maxRelDiff = FloatEpsilon) noexcept
    {
        if(a == b)
            return true;
        auto diff = std::fabs(a - b);
        if(diff <= maxAbsDiff)
            return true;
        auto aAbs = std::fabs(a);
        auto bAbs = std::fabs(b);
        auto max = (aAbs > bAbs) ? aAbs : bAbs;
        if(diff <= max * maxRelDiff)
            return true;
        return false;
    }
} //end namespace nc::math