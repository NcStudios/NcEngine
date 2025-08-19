/**
 * @file SpscQueue.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncutility/platform/Platform.h"

#include <array>
#include <atomic>
#include <new>

namespace nc
{
NC_DISABLE_WARNING_PUSH
NC_DISABLE_WARNING_MSVC(4324)

/** @brief Fixed-size single-producer/single-consumer queue. */
template <class T, size_t N>
class spsc_queue
{
    public:
        [[nodiscard]] auto push(const T& v) -> bool
        {
            return emplace(v);
        }

        [[nodiscard]] auto push(T&& v) -> bool
        {
            return emplace(std::move(v));
        }

        [[nodiscard]] auto pop(T& out) -> bool
        {
            if (is_empty(m_consumer))
            {
                refresh_consumer_cache();
                if (is_empty(m_consumer))
                {
                    return false;
                }
            }

            out = std::move(m_buffer[index(m_consumer.head)]);
            advance_consumer();
            return true;
        }

        template<class... Args>
        [[nodiscard]] auto emplace(Args&&... args) -> bool
        {
            if (is_full(m_producer))
            {
                refresh_producer_cache();
                if (is_full(m_producer))
                {
                    return false;
                }
            }

            m_buffer[index(m_producer.tail)] = T(std::forward<Args>(args)...);
            advance_producer();
            return true;
        }

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return m_head.load(std::memory_order_acquire) ==
                   m_tail.load(std::memory_order_acquire);
        }

    private:
        struct Cache
        {
            size_t head = 0;
            size_t tail = 0;
        };

        static_assert(N >= 1, "Queue capacity must be >= 1");
        static_assert(std::atomic<size_t>::is_always_lock_free);
        static constexpr auto CacheLine = std::hardware_destructive_interference_size;

        alignas(CacheLine) std::array<T, N> m_buffer = {};
        alignas(CacheLine) std::atomic<size_t> m_head = 0;
        alignas(CacheLine) std::atomic<size_t> m_tail = 0;
        alignas(CacheLine) Cache m_producer;
        alignas(CacheLine) Cache m_consumer;

        static constexpr auto index(size_t i) -> size_t
        {
            if constexpr ((N & (N - 1)) == 0)
            {
                return i & (N - 1);
            }
            else
            {
                return i % N;
            }
        }

        static auto is_full(const Cache& cache)  -> bool { return cache.tail - cache.head >= N; }
        static auto is_empty(const Cache& cache) -> bool { return cache.head == cache.tail; }
        void refresh_producer_cache()                    { m_producer.head = m_head.load(std::memory_order_acquire); }
        void refresh_consumer_cache()                    { m_consumer.tail = m_tail.load(std::memory_order_acquire); }
        void advance_producer()                          { m_tail.store(++m_producer.tail, std::memory_order_release); }
        void advance_consumer()                          { m_head.store(++m_consumer.head, std::memory_order_release); }
};

NC_DISABLE_WARNING_POP
} // namespace nc
