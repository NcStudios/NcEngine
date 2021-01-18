#pragma once

#include <cstdint>
#include <cstring>
#include <concepts>
#include <cmath>
#include <limits> //numeric_limits::epsilon

#include <iostream>

namespace nc::math
{
    constexpr auto FloatEpsilon = std::numeric_limits<float>::epsilon();
    constexpr auto DoubleEpsilon = std::numeric_limits<double>::epsilon();
    constexpr double PI_DOUBLE = atan(1)*4;
    constexpr float  PI_FLOAT  = PI_DOUBLE;

    template<std::totally_ordered T> constexpr T Min(T a, T b) noexcept { return a < b ? a : b; }
    template<std::totally_ordered T> constexpr T Max(T a, T b) noexcept { return a > b ? a : b; }

    constexpr double Ceiling(double val) noexcept { return (int)(val + 1); }
    constexpr double Floor  (double val) noexcept { return (int) val; }
    constexpr double Round  (double val) noexcept { return (int)(val + 0.5); }

    template<std::totally_ordered T> constexpr T Clamp(T val, T min, T max) noexcept
    {
        if(val < min)
            val = min;
        else if(val > max)
            val = max;
        return val;
    }

    constexpr double Lerp(double a, double b, double factor) noexcept { return a + (b - a) * factor; }

    constexpr double WrapAngle(double theta) noexcept
    {
        const double modTwoPi = fmod(theta, 2.0L * PI_DOUBLE);
        return (modTwoPi > PI_DOUBLE ? (modTwoPi - 2.0L * PI_DOUBLE) : modTwoPi);
    }

    constexpr double DegreeToRadian(double degrees) noexcept { return degrees * PI_DOUBLE / 180.0;     }
    constexpr double RadianToDegree(double radians) noexcept { return radians *     180.0 / PI_DOUBLE; }

    constexpr bool FloatEqual(float a, float b, float maxAbsDiff = 0.00001f, float maxRelDiff = FloatEpsilon) noexcept
    {
        if(a == b)
            return true;
        auto diff = fabs(a - b);
        if(diff <= maxAbsDiff)
            return true;
        auto aAbs = fabs(a);
        auto bAbs = fabs(b);
        auto max = (aAbs > bAbs) ? aAbs : bAbs;
        if(diff <= max * maxRelDiff)
            return true;
        return false;
    }
} //end namespace nc::math