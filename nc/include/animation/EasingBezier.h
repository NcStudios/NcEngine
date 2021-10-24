#pragma once

#include <string>

#include "animation/Easing.h"
#include "math/Vector.h"


namespace nc
{

    /**
     * An easing with two points that the user can define which defines a 
     * custom bezier path between {0,0} -> Point1 -> Point2 -> {1,1}.
     */
    class EasingBezier : public Easing
    {
    public:
        EasingBezier(const nc::Vector2 point1, const nc::Vector2 point2)
            : m_point1(point1), m_point2(point2) {};

        float Compute(float delta);
        const std::string Name() const { return "Bezier"; };
        nc::Vector2& Point1() { return m_point1; };
        nc::Vector2& Point2() { return m_point2; };
    private:
        nc::Vector2 m_point1;
        nc::Vector2 m_point2;
    };

}
