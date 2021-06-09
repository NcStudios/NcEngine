#include "random/Random.h"

#include <random>

namespace
{
    class FloatDistribution
    {
        public:
            FloatDistribution()
                : m_device{},
                  m_gen{m_device()},
                  m_distribution{-1.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())}
            {
            }

        float Generate()
        {
            return m_distribution(m_gen);
        }

        private:
            std::random_device m_device;
            std::mt19937 m_gen;
            std::uniform_real_distribution<float> m_distribution;
    };

    FloatDistribution g_floatDistribution;
}

namespace nc::random
{
    float Float()
    {
        return g_floatDistribution.Generate();
    }

    float Float(float offset, float range)
    {
        return offset + Float() * range;
    }

    Vector3 Vec3()
    {
        return Vector3{Float(), Float(), Float()};
    }

    Vector3 Vec3(Vector3 offset, Vector3 range)
    {
        return offset + HadamardProduct(Vec3(), range);
    }
}