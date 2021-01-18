#pragma once

#include <cstdint>
#include <cstring>
#include <concepts>
#include <cmath>
#include <limits> //numeric_limits::epsilon

#include <iostream>

namespace nc::math
{
    const auto FloatEpsilon = std::numeric_limits<float>::epsilon();
    const auto DoubleEpsilon = std::numeric_limits<double>::epsilon();

    const double PI_DOUBLE = atan(1)*4;
    const float  PI_FLOAT  = PI_DOUBLE;

    inline bool FloatEqual(float a, float b, float maxDiff = 0.00001f, float maxRelDiff = FloatEpsilon) noexcept
    {
        auto diff = fabs(a - b);
        if(diff <= maxDiff)
            return true;

        auto aAbs = fabs(a);
        auto bAbs = fabs(b);
        auto max = (aAbs > bAbs) ? aAbs : bAbs;

        if(diff <= max * maxRelDiff)
            return true;
        
        return false;
    }

    template<std::totally_ordered T> inline T Min(T a, T b) noexcept { return a < b ? a : b; }
    template<std::totally_ordered T> inline T Max(T a, T b) noexcept { return a > b ? a : b; }

    inline double Ceiling(double val)     noexcept { return (int)(val + 1); }
    inline double Floor  (double val)     noexcept { return (int) val; }
    inline double Round  (double val)     noexcept { return (int)(val + 0.5); }

    template<std::totally_ordered T> inline T Clamp(T val, T min, T max) noexcept
    {
        if(val < min)
            val = min;
        else if(val > max)
            val = max;
        return val;
    }

    inline double Lerp(double a, double b, double factor) noexcept { return a + (b - a) * factor; }

    inline double WrapAngle(double theta) noexcept
    {
        const double modTwoPi = fmod(theta, 2.0L * PI_DOUBLE);
        return (modTwoPi > PI_DOUBLE ? (modTwoPi - 2.0L * PI_DOUBLE) : modTwoPi);
    }

    inline double DegreeToRadian(double degrees) noexcept { return degrees * PI_DOUBLE / 180.0;     }
    inline double RadianToDegree(double radians) noexcept { return radians *     180.0 / PI_DOUBLE; }

} //end namespace nc::math