/**
 * @file PhysicsTick.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */

#pragma once

#include <compare>
#include <cstdint>
#include <limits>

namespace nc
{
/** @brief Type safe wrapper representing a timepoint in the physics simulation. */
class PhysicsTick
{
    public:
        using tick_t = uint32_t;
        static constexpr auto NullTick = std::numeric_limits<tick_t>::max();

        constexpr explicit PhysicsTick() noexcept
            : m_value{NullTick} {}

        constexpr explicit PhysicsTick(tick_t tick) noexcept
            : m_value{tick} {}

        constexpr static auto Null()                         noexcept -> PhysicsTick  { return PhysicsTick{}; }
        constexpr auto IsNull()                        const noexcept -> bool         { return m_value == NullTick; }
        constexpr auto Value()                         const noexcept -> tick_t       { return m_value; }
        constexpr explicit operator tick_t()           const noexcept                 { return m_value; }
        constexpr auto operator++()                          noexcept -> PhysicsTick& { ++m_value; return *this; }
        constexpr auto operator++(int)                       noexcept -> PhysicsTick  { auto temp = *this; ++(*this); return temp;}
        constexpr auto operator--()                          noexcept -> PhysicsTick& { --m_value; return *this; }
        constexpr auto operator--(int)                       noexcept -> PhysicsTick  { auto temp = *this; --(*this); return temp;}
        constexpr auto operator+ (tick_t rhs)          const noexcept -> PhysicsTick  { return PhysicsTick(m_value + rhs); }
        constexpr auto operator- (tick_t rhs)          const noexcept -> PhysicsTick  { return PhysicsTick(m_value - rhs); }
        constexpr auto operator+=(tick_t rhs)                noexcept -> PhysicsTick& { m_value += rhs; return *this; }
        constexpr auto operator-=(tick_t rhs)                noexcept -> PhysicsTick& { m_value -= rhs; return *this; }
        constexpr auto operator<=>(const PhysicsTick&) const noexcept = default;

    private:
        tick_t m_value;
};
} // namespace nc
