#pragma once

#include <random>


namespace nc::random
{

    /** Random number generator that can be seeded and passwed around to control deterministic behavior. */
    class Random
    {
    public:
        Random();
        Random(unsigned int seed);
        
        float Get();                                // 0 -> 1
        float After(float offset, float range);     // offset -> offset + range
        float Between(float min, float max);        // min -> max

        unsigned int GetSeed() const;
        void SetSeed(unsigned int seed);

    private:
        unsigned int m_seed;
        std::mt19937 m_gen;
        std::uniform_real_distribution<float> m_distribution;
    };
    
}