#pragma once

#include "Module.h"
#include "Vector.h"
#include "Xoshiro256.h"

#include <random>

namespace nc
{
    /**
     * @brief Random number generator.
     * 
     * A shared Random instance can be retrieved from the NcEngine pointer. Additional
     * instances can be constructed, copied, and forked as needed.
     * 
     */
    class Random : public Module
    {
        public:
            /**
             * @brief Default construct a new Random object
             */
            explicit Random() noexcept
                : Random(std::random_device{}())
            {
            }

            /**
             * @brief Construct a new Random object from a seed.
             * @param seed A 64 bit seed value
             */
            explicit Random(size_t seed) noexcept
                : m_seed{seed},
                  m_generator{m_seed},
                  m_distribution{0.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())}
            {
            }

            /**
             * @brief Returns a new Random instance seeded with the next generator value.
             * @return size_t
             */
            auto Fork() noexcept { return Random{GetU64()}; }

            /**
             * @brief Seed the generator
             * @param seed A 64 bit seed value.
             */
            void Seed(size_t seed) noexcept { *this = Random{seed}; }

            /**
             * @brief Get the current seed.
             * @return size_t
             */
            auto Seed() const noexcept { return m_seed; }

            /**
             * @brief Generate a random float in the range [0, 1].
             * @return float
             */
            auto Get() noexcept -> float { return m_distribution(m_generator); }

            /**
             * @brief Generate a random size_t in the range [0, std::numeric_limits<size_t>::max()].
             * @return size_t
             */
            auto GetU64() noexcept -> size_t { return m_generator(); }

            /**
             * @brief Generate a random Vector2 with components in the range [0, 1].
             * @return Vector2
             */
            auto GetVector2() noexcept -> Vector2 { return Vector2{Get(), Get()}; }

            /**
             * @brief Generate a random Vector3 with components in the range [0, 1].
             * @return Vector3
             */
            auto GetVector3() noexcept -> Vector3 { return Vector3{Get(), Get(), Get()}; }

            /**
             * @brief Generate a random Vector4 with components in the range [0, 1].
             * @return Vector4
             */
            auto GetVector4() noexcept -> Vector4 { return Vector4{Get(), Get(), Get(), Get()}; }

            /**
             * @brief Generate a random float in the range [min, max].
             * @param min The minimum range value.
             * @param max The maximum range value.
             * @return float
             */
            auto Between(float min, float max) noexcept -> float { return Get() * (max - min) + min; }

            /**
             * @brief Generate a random size_t in the range [min, max].
             * @param min The minimum range value.
             * @param max The maximum range value.
             * @return size_t
             */
            auto Between(size_t min, size_t max) noexcept -> size_t { return m_generator() * (max - min) + min; }

            /**
             * @brief Generate a random Vector2 with components in the range [min, max].
             * @param min The component-wise minimum range values.
             * @param max The component-wise maximum range values.
             * @return Vector2
             */
            auto Between(const Vector2& min, const Vector2& max) noexcept -> Vector2 { return HadamardProduct(GetVector2(), max - min) + min; }

            /**
             * @brief Generate a random Vector3 with components in the range [min, max].
             * @param min The component-wise minimum range values.
             * @param max The component-wise maximum range values.
             * @return Vector3
             */
            auto Between(const Vector3& min, const Vector3& max) noexcept -> Vector3 { return HadamardProduct(GetVector3(), max - min) + min; }

            /**
             * @brief Generate a random Vector4 with components in the range [min, max].
             * @param min The component-wise minimum range values.
             * @param max The component-wise maximum range values.
             * @return Vector4
             */
            auto Between(const Vector4& min, const Vector4& max) noexcept -> Vector4 { return HadamardProduct(GetVector4(), max - min) + min; }

            /**
             * @brief Check if two generators have the same state.
             * @return bool
             */
            friend bool operator==(const Random& lhs, const Random& rhs) noexcept { return lhs.m_generator == rhs.m_generator; }

            /**
             * @brief Check if two generators have different states.
             * @return bool
             */
            friend bool operator!=(const Random& lhs, const Random& rhs) noexcept { return lhs.m_generator != rhs.m_generator; }

        private:
            size_t m_seed;
            Xoshiro256ss m_generator;
            std::uniform_real_distribution<float> m_distribution;
    };
}