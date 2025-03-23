/**
 * @file SupportFunctions.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include <memory>

namespace nc
{
class CookedShape;
struct Vector3;

/** @brief Wrapper around a scratch buffer used by support functions. */
class SupportBuffer
{
    struct Impl;

    public:
        /** @brief Get the global SupportBuffer for use with single-threaded calls to support functions. */
        static auto GetDefault() -> SupportBuffer&;

        /** @brief Create a new SupportBuffer for use with multi-threaded calls to support functions. */
        SupportBuffer();

        SupportBuffer(SupportBuffer&& other) noexcept;
        SupportBuffer& operator=(SupportBuffer&& other) noexcept;
        SupportBuffer(const SupportBuffer&) = delete;
        SupportBuffer& operator=(const SupportBuffer&) = delete;
        ~SupportBuffer() noexcept;

        /** @cond internal */
        auto GetImpl() -> Impl& { return *m_impl; }
        /** @endcond internal */

    private:
        std::unique_ptr<Impl> m_impl;
};

/**
 * @brief Get the furthest vertex in a direction in world space.
 * @note Concurrent calls to this function require unique SupportBuffers.
 */
auto GetWorldSupport(const CookedShape& shape,
                     const Vector3& directionNormal,
                     SupportBuffer& buffer = SupportBuffer::GetDefault()) -> Vector3;

/**
 * @brief Get the shape's half extent in a direction.
 * @note Concurrent calls to this function require unique SupportBuffers.
 */
auto GetHalfExtent(const CookedShape& shape,
                   const Vector3& directionNormal,
                   SupportBuffer& buffer = SupportBuffer::GetDefault()) -> float;
} // namespace nc
