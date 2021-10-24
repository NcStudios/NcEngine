#pragma once

#include <functional>


namespace nc
{

    /** A linear range of values */
    template <typename T>
    class Range
    {
        using lerp_type = std::function<T(const T&, const T&, float delta)>;
    public:
        
        Range(const T min, const T max, lerp_type lerp)
            : m_min(min), m_max(max), m_lerp(lerp) {};
        Range(const T min, const T max)
            : m_min(min), m_max(max), 
              m_lerp([](const T& s, const T& e, float delta) { return (1 - delta) * s + delta * e; }) {};
        Range(const T value)
            : Range(value, value) {};

        T At(float delta) const { return m_lerp ? m_lerp(m_min, m_max, delta) : T(); };

        T& Min() { return m_min; };
        T& Max() { return m_max; };
    private:
        T m_min;
        T m_max;
        lerp_type m_lerp;
    };

}