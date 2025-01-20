/**
 * @file PhysicsSnapshot.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/physics/PhysicsTick.h"

#include <any>
#include <memory>
#include <span>
#include <vector>

namespace nc
{
/** @brief Holds serialized physics state for performing physics simulation rollbacks. */
class PhysicsSnapshot final
{
    public:
        PhysicsSnapshot();
        PhysicsSnapshot(PhysicsTick tick, std::vector<uint8_t> bytes);
        PhysicsSnapshot(PhysicsSnapshot&&) noexcept;
        PhysicsSnapshot& operator=(PhysicsSnapshot&&) noexcept;
        ~PhysicsSnapshot() noexcept;

        /** @brief Check if the snapshot has recorded state. */
        auto IsValid() const -> bool { return !m_tick.IsNull(); }

        /** @brief Get the PhysicsTick the snapshot was taken at, or PhysicsTick::Null() if invalid. */
        auto GetTick() const -> PhysicsTick { return m_tick; }

        /** @brief Get the size of the snapshot in bytes. */
        auto GetSize() const -> size_t;

        /** @brief Get a view over the serialized bytes. */
        auto ViewBuffer() const -> std::span<const uint8_t>;

        /** @brief Extract the serialized bytes, clearing the snapshot. */
        auto ExtractBuffer() -> std::vector<uint8_t>;

        /** @brief Set the read position back to the beginning of the snapshot. */
        void ResetRead();

        /** @brief Clear any recorded state. */
        void Clear();

        /** @cond internal */
        void Save(std::any physicsSystem, PhysicsTick tick);
        auto Restore(std::any physicsSystem) -> bool;
        void SetValidationMode(bool enabled);
        /** @endcond internal */

    protected:
        class Impl;
        std::unique_ptr<Impl> m_impl;
        PhysicsTick m_tick = PhysicsTick::Null();
};
} // namespace nc
