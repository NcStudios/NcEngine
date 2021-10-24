#pragma once

#include <functional>
#include <string>


namespace nc
{

    using EasingCompute = std::function<float(float)>;

    /** 
     * An easing is a function which defines the velocity for an animation given 
     * a point in time "delta" which is between 0 and 1 representing the start 
     * and end of an animation. The simplest type of easing is a linear easing 
     * which has a constant velocity - in other words the delta given is the 
     * delta returned. Easing functions can return values outsiide the bounds of 
     * 0 and 1 which animates before the start or after the end.
     */
    class Easing
    {
    public:
        virtual float Compute(float delta) const = 0;
        virtual const std::string Name()   const = 0;

        // TODO serialize function in here or separate place
    };

}
