/**
 * @file PhysicsSnapshot.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include <limits>
#include <memory>
#include <vector>

namespace nc
{
namespace physics
{
class PhysicsSnapshotImpl;
} // namespace physics

/** @brief Type safe wrapper representing a timepoint in the physics simulation. */
class PhysicsTick
{
    public:
        using tick_t = uint32_t;
        static constexpr auto NullTick = std::numeric_limits<tick_t>::max();

        constexpr explicit PhysicsTick()            noexcept : m_value{NullTick} {}
        constexpr explicit PhysicsTick(tick_t tick) noexcept : m_value{tick}     {}

        static constexpr auto Null() -> PhysicsTick { return PhysicsTick{}; }

        constexpr explicit operator tick_t()           const noexcept                 { return m_value; }
        constexpr auto Value()                         const noexcept -> tick_t       { return m_value; }
        constexpr auto IsNull()                        const noexcept -> bool         { return m_value == NullTick; }
        constexpr auto operator++()                          noexcept -> PhysicsTick& { ++m_value; return *this; }
        constexpr auto operator++(int)                       noexcept -> PhysicsTick  { auto temp = *this; ++(*this); return temp;}
        constexpr auto operator--()                          noexcept -> PhysicsTick& { --m_value; return *this; }
        constexpr auto operator--(int)                       noexcept -> PhysicsTick  { auto temp = *this; --(*this); return temp;}
        constexpr auto operator+(tick_t rhs)           const noexcept -> PhysicsTick  { return PhysicsTick(m_value + rhs); }
        constexpr auto operator-(tick_t rhs)           const noexcept -> PhysicsTick  { return PhysicsTick(m_value - rhs); }
        constexpr auto operator+=(tick_t rhs)                noexcept -> PhysicsTick& { m_value += rhs; return *this; }
        constexpr auto operator-=(tick_t rhs)                noexcept -> PhysicsTick& { m_value -= rhs; return *this; }
        constexpr auto operator<=>(const PhysicsTick&) const noexcept = default;

    private:
        tick_t m_value;
};


/** @brief */
class PhysicsSnapshot final
{
    public:
        PhysicsSnapshot();
        PhysicsSnapshot(PhysicsSnapshot&&) noexcept;
        PhysicsSnapshot& operator=(PhysicsSnapshot&&) noexcept;
        ~PhysicsSnapshot() noexcept;

        // todo: - get/extract buffer, compress?
        //       - reserve ?

        /** @brief Check if the snapshot has recorded state. */
        auto IsValid() const -> bool;

        /** @brief Get the PhysicsTick the snapshot was taken at, or PhysicsTick::Null() if the snapshot is invalid. */
        auto GetTick() const -> PhysicsTick;

        /** @brief Get the size of the snapshot in bytes. */
        auto GetSize() const -> size_t;

        /** @brief Set the read position back to the beginning of the snapshot. */
        void ResetRead();

        /**
         * @brief Clear any recorded state.
         * @note Snapshots can be reused to minimize allocations, but should be cleared before saving new state.
         */
        void Clear();

        /** @brief  */
        void SetValidationMode(bool enabled);

        /** @cond internal */
        auto GetImpl()        -> physics::PhysicsSnapshotImpl&;
        /** @endcond internal */

    protected:
        std::unique_ptr<physics::PhysicsSnapshotImpl> m_impl;
};
} // namespace nc
