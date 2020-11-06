#pragma once
#include <cmath>
#include <limits> //numeric_limits::epsilon

namespace nc::math
{
    const double EPSILON = std::numeric_limits<float>::epsilon();

    const double PI_DOUBLE = atan(1)*4;
    const float  PI_FLOAT  = PI_DOUBLE;

    template<class T> inline T Min(T a, T b) noexcept { return a < b ? a : b; }
    template<class T> inline T Max(T a, T b) noexcept { return a > b ? a : b; }

    inline double Ceiling(double val)     noexcept { return (int)(val + 1); }
    inline double Floor  (double val)     noexcept { return (int) val; }
    inline double Round  (double val)     noexcept { return (int)(val + 0.5); }

    template<class T> inline T Clamp(T val, T min, T max) noexcept
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