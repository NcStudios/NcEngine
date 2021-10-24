#pragma once

#include <string>

#include "animation/Easing.h"


namespace nc
{

    /**
     * A simple easing that's only the function.
     */
    class EasingFunction : public Easing
    {
    public:
        EasingFunction(const std::string& name, EasingCompute func)
            : m_name(name), m_func(std::move(func)) {};

        float Compute(float delta) const override { return m_func(delta); };
        const std::string Name() const override { return m_name; };
        const EasingCompute GetFunc() const { return m_func; };
    private:
        const std::string m_name;
        const EasingCompute m_func;
    };

}
