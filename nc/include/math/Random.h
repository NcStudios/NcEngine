#pragma once

#include "Vector.h"
#include "Xoshiro256.h"

#include <random>

namespace nc
{
    /** Random number generator. A shared instance can be retreived from the NcEngine
     *  pointer. Additional instances can be constructed, copied, and forked as needed. */
    class Random
    {
        public:
            explicit Random() noexcept
                : Random(std::random_device{}())
            {
            }

            explicit Random(size_t seed) noexcept
                : m_seed{seed},
                  m_generator{m_seed},
                  m_distribution{0.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())}
            {
            }

            /** Returns a new Random instance seeded with the next generator value. */
            auto Fork() noexcept { return Random{GetU64()}; }

            /** Seed the generator. */
            void Seed(size_t seed) noexcept { *this = Random{seed}; }

            /** Get the current seed. */
            auto Seed() const noexcept { return m_seed; }

            /** Returns a random float in the range [0, 1]. */
            auto Get() noexcept -> float { return m_distribution(m_generator); }

            /** Returns a random size_t in the range [0, std::numeric_limits<size_t>::max()]. */
            auto GetU64() noexcept -> size_t { return m_generator(); }

            /** Returns a random Vector2 with components in the range [0, 1] */
            auto GetVector2() noexcept -> Vector2 { return Vector2{Get(), Get()}; }

            /** Returns a random Vector3 with components in the range [0, 1] */
            auto GetVector3() noexcept -> Vector3 { return Vector3{Get(), Get(), Get()}; }

            /** Returns a random Vector4 with components in the range [0, 1] */
            auto GetVector4() noexcept -> Vector4 { return Vector4{Get(), Get(), Get(), Get()}; }

            /** Returns a random float in the range [min, max]. */
            auto Between(float min, float max) noexcept -> float { return Get() * (max - min) + min; }

            /** Returns a random size_t in the range [min, max]. */
            auto Between(size_t min, size_t max) noexcept -> size_t { return m_generator() * (max - min) + min; }

            /** Returns a random Vector2 with components in the range [min, max]. */
            auto Between(const Vector2& min, const Vector2& max) noexcept -> Vector2 { return HadamardProduct(GetVector2(), max - min) + min; }

            /** Returns a random Vector3 with components in the range [min, max]. */
            auto Between(const Vector3& min, const Vector3& max) noexcept -> Vector3 { return HadamardProduct(GetVector3(), max - min) + min; }

            /** Returns a random Vector4 with components in the range [min, max]. */
            auto Between(const Vector4& min, const Vector4& max) noexcept -> Vector4 { return HadamardProduct(GetVector4(), max - min) + min; }

            /** Returns true if the lhs and rhs generators are in identical states. */
            friend bool operator==(const Random& lhs, const Random& rhs) noexcept { return lhs.m_generator == rhs.m_generator; }

            /** Returns true if the lhs and rhs generators are in different states. */
            friend bool operator!=(const Random& lhs, const Random& rhs) noexcept { return lhs.m_generator != rhs.m_generator; }

        private:
            size_t m_seed;
            Xoshiro256ss m_generator;
            std::uniform_real_distribution<float> m_distribution;
    };
}