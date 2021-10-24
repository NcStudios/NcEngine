#pragma once

#include <functional>
#include <string>

#include "animation/Easing.h"


namespace nc
{

    class EasingModifier;

    using EasingComputeFactory = std::function<EasingCompute(Easing*)>;
    using EasingModifierFactory = std::function<EasingModifier(Easing*)>;

    /**
     * An easing which modifies another easing with a simple function. This can
     * be used to change the direction, mirror, yoyo, etc another easing.
     */
    class EasingModifier : public Easing
    {
    public:
        EasingModifier(const std::string& name, Easing* easing, EasingCompute func)
            : m_name(name), m_easing(easing), m_func(std::move(func)) {};

        float Compute(float delta)     const override { return m_func(delta); };
        const std::string Name()       const override { return m_name; };
        const Easing* GetEasing()      const { return m_easing; };
        const EasingCompute GetFunc()  const { return m_func; };

        static EasingModifierFactory Factory(const std::string& name, EasingComputeFactory compute) {
            return [name, compute = std::move(compute)](Easing* easing) {
                return EasingModifier(name, easing, compute(easing));
            };
        };
        
    private:
        const std::string m_name;
        const Easing* m_easing;
        const EasingCompute m_func;
    };

}