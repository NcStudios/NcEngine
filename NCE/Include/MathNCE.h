#ifndef MATH_NCE_H
#define MATH_NCE_H

#include <cmath>

namespace nc::math
{
    struct MathNCE
    {
        //static const double PI = atan(1)*4;

        //variadic template to support any number?
        static double Min(double a, double b) { return a < b ? a : b; }
        static double Max(double a, double b) { return a > b ? a : b; }

        //static float Pow(float num, float )

        static double Floor(double val) { return (int)val; }
        static double Ceiling(double val) { return (int)(val + 1); }
        static double Round(double val) { return (int)(val + 0.5); }
        static double Clamp(double val, double min, double max)
        {
            if(val < min)
                val = min;
            else if(val > max)
                val = max;
            return val;
        }

        static double Lerp(double a, double b, double factor)
        {
            return a + (b - a) * factor;
        }

        //static double DegreeToRadian;;
        //static double RadianToDegree;
    };
} //end namespace nc::math

#endif