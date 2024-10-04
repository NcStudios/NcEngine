/**
 * @file Math.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstdint>
#include <concepts>
#include <cmath>
#include <limits>
#include <numbers>

namespace nc
{
constexpr auto FloatEpsilon = std::numeric_limits<float>::epsilon();
constexpr auto DoubleEpsilon = std::numeric_limits<double>::epsilon();

template<std::totally_ordered T>
constexpr T Min(T a, T b) noexcept
{
    return a < b ? a : b;
}

template<std::totally_ordered T>
constexpr T Max(T a, T b) noexcept
{
    return a > b ? a : b;
}

template<std::totally_ordered T>
constexpr T Clamp(T val, T min, T max) noexcept
{
    if(val < min)
        val = min;
    else if(val > max)
        val = max;
    return val;
}

template<class T, std::floating_point U>
    requires requires (T t, U u) { t + t; t - t; t * u; }
constexpr T Lerp(T from, T to, U factor) noexcept
{
    return from + (to - from) * Clamp(factor, 0.0f, 1.0f);
}

template<std::floating_point T>
constexpr T WrapAngle(T theta) noexcept
{
    constexpr auto twoPi = static_cast<T>(2.0L * std::numbers::pi);
    const T modTwoPi = std::fmod(theta, twoPi);
    return (modTwoPi > std::numbers::pi ? (modTwoPi - twoPi) : modTwoPi);
}

template<std::floating_point T>
constexpr T DegreesToRadians(T degrees) noexcept
{
    return degrees * std::numbers::pi_v<T> / 180.0f;
}

template<std::floating_point T>
constexpr T RadiansToDegrees(T radians) noexcept
{
    return radians * 180.0f / std::numbers::pi_v<T>;
}

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
} // namespace nc
