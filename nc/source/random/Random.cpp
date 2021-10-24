#include "random/Random.h"

#include <random>


namespace nc::random
{

    /** Random Implementation */

    Random::Random(unsigned int seed) 
        : m_seed(seed),
          m_gen{seed},
          m_distribution{0.0f, 1.0f}
    { }

    Random::Random(): Random(std::random_device{}()) {}

    float Random::Get()
    {
        return m_distribution(m_gen);
    }

    float Random::After(float offset, float range)
    {
        return Get() * range + offset;
    }

    float Random::Between(float min, float max)
    {
        return Get() * (max - min) + min;
    }

    unsigned int Random::GetSeed() const
    {
        return m_seed;
    }

    void Random::SetSeed(unsigned int seed)
    {
        m_seed = seed;
        m_gen.seed(seed);
    }

}