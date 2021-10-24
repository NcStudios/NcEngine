#include "animation/EasingBezier.h"


namespace nc
{

    /** EasingBezier Implementation */
    float A(float aA1, float aA2)
    {
        return 1.0 - 3.0 * aA2 + 3.0 * aA1;
    }
    float B(float aA1, float aA2)
    {
        return 3.0 * aA2 - 6.0 * aA1;
    }
    float C(float aA1)
    {
        return 3.0 * aA1;
    }
    float CalcBezier(float aT, float aA1, float aA2)
    {
        return ((A(aA1, aA2)*aT + B(aA1, aA2))*aT + C(aA1))*aT;
    }
    float GetSlope(float aT, float aA1, float aA2)
    {
        return 3.0 * A(aA1, aA2)*aT*aT + 2.0 * B(aA1, aA2) * aT + C(aA1);
    }
    float GetTForX(float aX, float x1, float x2)
    {
        // Newton raphson iteration
        float aGuessT = aX;

        for (uint8_t i = 0; i < 4; ++i) 
        {
            float currentSlope = GetSlope(aGuessT, x1, x2);

            if (currentSlope == 0.0f) 
            {
                return aGuessT;
            }

            float currentX = CalcBezier(aGuessT, x1, x2) - aX;

            aGuessT -= currentX / currentSlope;
        }

        return aGuessT;
    }

    float EasingBezier::Compute(float x)
    {
        return CalcBezier( GetTForX( x, m_point1.x, m_point2.x ), m_point1.y, m_point2.y );
    }

}

