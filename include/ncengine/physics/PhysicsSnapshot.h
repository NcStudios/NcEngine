/**
 * @file PhysicsSnapshot.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/physics/PhysicsTick.h"

#include <memory>
#include <vector>

namespace nc
{
namespace physics
{
class PhysicsSnapshotImpl;
} // namespace physics

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
        auto GetImpl() -> physics::PhysicsSnapshotImpl&;
        /** @endcond internal */

    protected:
        std::unique_ptr<physics::PhysicsSnapshotImpl> m_impl;
};
} // namespace nc
