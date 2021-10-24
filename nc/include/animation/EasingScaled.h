#pragma once

#include <functional>
#include <string>

#include "animation/Easing.h"


namespace nc
{

    /**
     * An easing which scales the velocity of another easing. This easing 
     * basically exagerrates the easing relative to the direct path from 0 to 1.
     */
    class EasingScaled : public Easing
    {
    public:
        EasingScaled(float scale, Easing* easing)
            : m_scale(scale), m_easing(easing) {};

        float Compute(float x) const override { return m_scale * m_easing->Compute(x) + (1 - m_scale) * x; };
        const std::string Name() const override { return "Scale"; };

        Easing* GetEasing() const { return m_easing; };
        void SetEasing(Easing* easing) { m_easing = easing; };

        float GetScale() const { return m_scale; };
        void SetScale(float scale) { m_scale = scale; };
    private:
        float m_scale;
        Easing* m_easing;
    };

}