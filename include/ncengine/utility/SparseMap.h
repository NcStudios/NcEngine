#pragma once

#include "ncutility/NcError.h"

#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

namespace nc
{
/** @brief A map-like container with O(1) insertion, removal, and lookup and O(N) iteration. */
template<std::movable Value, std::convertible_to<uint32_t> Key = uint32_t>
class sparse_map
{
    public:
        static constexpr auto NullIndex = UINT32_MAX;

        explicit sparse_map(size_t initialKeyCapacity, size_t maxKeyCapacity = 65536ull)
            : m_sparse(initialKeyCapacity, NullIndex),
              m_initialKeyCapacity{initialKeyCapacity},
              m_maxKeyCapacity{maxKeyCapacity}
        {
        }

        auto emplace(Key key, const Value& value) -> Value&
        {
            ensure_capacity(key);
            auto& denseIndex = m_sparse.at(static_cast<size_t>(key));
            NC_ASSERT(denseIndex == NullIndex, "Key already exists");
            denseIndex = static_cast<uint32_t>(m_dense.size());
            m_dense.push_back(key);
            m_values.push_back(value);
            return m_values.back();
        }

        auto emplace(Key key, Value&& value) -> Value&
        {
            ensure_capacity(key);
            auto& denseIndex = m_sparse.at(static_cast<size_t>(key));
            NC_ASSERT(denseIndex == NullIndex, "Key already exists");
            denseIndex = static_cast<uint32_t>(m_dense.size());
            m_dense.push_back(key);
            m_values.push_back(std::move(value));
            return m_values.back();
        }

        auto erase(Key key) -> bool
        {
            if (!contains(key))
                return false;

            const auto normalizedKey = static_cast<size_t>(key);
            const auto swappedSparse = m_dense.back();
            const auto toRemoveDense = m_sparse.at(normalizedKey);
            m_sparse.at(swappedSparse) = static_cast<uint32_t>(key);
            m_sparse.at(normalizedKey) = NullIndex;
            m_dense.at(toRemoveDense) = m_dense.back();
            m_dense.pop_back();
            m_values.at(toRemoveDense) = std::move(m_values.back());
            m_values.pop_back();
            return true;
        }

        auto contains(Key key) const noexcept -> bool
        {
            const auto normalizedKey = static_cast<size_t>(key);
            return normalizedKey < m_sparse.size()
                ? m_sparse[normalizedKey] != NullIndex
                : false;
        }

        void reserve(size_t capacity)
        {
            m_dense.reserve(capacity);
            m_values.reserve(capacity);
        }

        void reserve_keys(size_t capacity)
        {
            m_sparse.reserve(capacity);
        }

        void clear()
        {
            m_sparse = std::vector<uint32_t>(m_initialKeyCapacity, NullIndex);
            m_dense.clear();
            m_dense.shrink_to_fit();
            m_values.clear();
            m_values.shrink_to_fit();
        }

        auto at(Key key) -> Value& { return m_values.at(m_sparse.at(key)); }
        auto at(Key key) const -> const Value& { return m_values.at(m_sparse.at(key)); }
        auto keys() noexcept -> std::span<Key> { return m_dense; }
        auto keys() const noexcept -> std::span<const Key> { return m_dense; }
        auto values() noexcept -> std::span<Value> { return m_values; }
        auto values() const noexcept -> std::span<const Value> { return m_values; }
        auto size() const noexcept -> size_t { return m_values.size(); }
        auto size_keys() const noexcept { return m_sparse.size(); }
        auto capacity() const noexcept { return m_values.capacity(); }
        auto capacity_keys() const noexcept { return m_sparse.capacity(); }

        auto begin() { return m_values.begin(); }
        auto begin() const { return m_values.begin(); }
        auto cbegin() const { return m_values.cbegin(); }
        auto end() { return m_values.end(); }
        auto end() const { return m_values.end(); }
        auto cend() const { return m_values.cend(); }

    private:
        std::vector<uint32_t> m_sparse;
        std::vector<Key> m_dense;
        std::vector<Value> m_values;
        size_t m_initialKeyCapacity;
        size_t m_maxKeyCapacity;

        auto dense_index(Key key) const -> uint32_t
        {
            return m_sparse.at(static_cast<size_t>(key));
        }

        void ensure_capacity(Key key)
        {
            const auto normalizedKey = static_cast<size_t>(key);
            const auto currentSize = m_sparse.size();
            if (normalizedKey >= currentSize)
            {
                if (normalizedKey > m_maxKeyCapacity)
                    throw NcError{"Max key capacity exceeded"};

                const auto growthSize = std::min<size_t>(currentSize * 2ull, m_maxKeyCapacity);
                const auto newSize = std::max<size_t>(growthSize, normalizedKey + 1ull);
                m_sparse.resize(newSize, NullIndex);
            }
        }
};
} // namespace nc
