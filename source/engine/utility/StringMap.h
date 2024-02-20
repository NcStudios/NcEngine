#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

namespace nc
{
struct DefaultStringTablePolicy
{
    static constexpr auto StableOrder = true;
    static constexpr auto PreserveOriginalKeys = true;
};

template<class Policy>
class BasicStringTable
{
    public:
        static constexpr auto NullIndex = UINT64_MAX;

        static auto hash(std::string_view key) noexcept
        {
            return std::hash<std::string_view>{}(key);
        }

        void emplace(std::string_view key)
        {
            m_hashes.push_back(hash(key));
            if constexpr (Policy::PreserveOriginalKeys)
                m_keys.push_back(std::string{key});
        }

        auto contains(std::string_view key) const noexcept -> bool
        {
            return std::ranges::contains(m_hashes, hash(key));
        }

        auto index(std::string_view key) const noexcept -> size_t
        {
            return index(hash(key));
        }

        auto index(size_t hash) const noexcept -> size_t
        {
            const auto pos = std::ranges::find(m_hashes, hash);
            return pos != std::ranges::cend(m_hashes)
                ? std::ranges::distance(std::ranges::begin(m_hashes), pos)
                : NullIndex;
        }

        auto erase(std::string_view key) noexcept -> bool
        {
            return erase(index(key));
        }

        auto erase(size_t index) noexcept -> bool
        {
            if (index >= m_hashes.size())
                return false;

            if constexpr (Policy::StableOrder)
            {
                m_hashes.erase(m_hashes.begin() + index);
                if constexpr (Policy::PreserveOriginalKeys)
                    m_keys.erase(m_keys.begin() + index);
            }
            else
            {
                if (index != m_hashes.size() - 1)
                {
                    m_hashes[index] = m_hashes.back();
                    m_hashes.pop_back();
                    if constexpr (Policy::PreserveOriginalKeys)
                    {
                        m_keys[index] = std::move(m_keys.back());
                        m_keys.pop_back();
                    }
                }
                else
                {
                    m_hashes.pop_back();
                    if constexpr (Policy::PreserveOriginalKeys)
                        m_keys.pop_back();
                }
            }

            return true;
        }

        void reserve(size_t count)
        {
            m_hashes.reserve(count);
            if constexpr (Policy::PreserveOriginalKeys)
                m_keys.reserve(count);
        }

        auto keys() const noexcept -> const std::vector<std::string>&
            requires Policy::PreserveOriginalKeys
        {
            return m_keys;
        }

        void clear() noexcept
        {
            m_hashes.clear();
            m_hashes.shrink_to_fit();
            if constexpr (Policy::PreserveOriginalKeys)
            {
                m_keys.clear();
                m_keys.shrink_to_fit();
            }
        }

        auto at(size_t index) const -> std::string_view
        {
            NC_ASSERT(index <= m_keys.size(), fmt::format("StringTable index out of bounds '{}'", index));
            return m_keys[index];
        }

        auto empty() const noexcept -> bool { return m_hashes.empty(); }
        auto size() const noexcept -> size_t { return m_hashes.size(); }

    private:
        std::vector<size_t> m_hashes;
        std::vector<std::string> m_keys;
};

using StringTable = BasicStringTable<DefaultStringTablePolicy>;

template<class T, class Policy = DefaultStringTablePolicy>
class StringMap
{
    public:
        static constexpr auto NullIndex = UINT64_MAX;

        static auto hash(std::string_view key) noexcept -> size_t
        {
            return BasicStringTable<Policy>::hash(key);
        }

        auto emplace(std::string_view key, T&& value) -> T&
        {
            m_table.emplace(key);
            m_values.push_back(std::move(value));
            return m_values.back();
        }

        auto emplace(std::string_view key, const T& value) -> T&
        {
            m_table.emplace(key);
            m_values.push_back(value);
            return m_values.back();
        }

        auto contains(std::string_view key) const noexcept -> bool
        {
            return m_table.contains(key);
        }

        auto index(std::string_view key) const noexcept -> size_t
        {
            return m_table.index(key);
        }

        auto index(size_t hash) const noexcept -> size_t
        {
            return m_table.index(hash);
        }

        auto erase(std::string_view key) noexcept -> bool
        {
            const auto index = m_table.index(key);
            if (index == BasicStringTable<Policy>::NullIndex)
                return false;

            m_table.erase(index);
            if constexpr (Policy::StableOrder)
            {
                m_values.erase(m_values.begin() + index);
            }
            else
            {
                if (index != m_values.size() - 1)
                {
                    m_values[index] = std::move(m_values.back());
                    m_values.pop_back();
                }
                else
                {
                    m_values.pop_back();
                }
            }

            return true;
        }

        void reserve(size_t count)
        {
            m_table.reserve(count);
            m_values.reserve(count);
        }

        auto keys() const noexcept -> const std::vector<std::string>&
            requires Policy::PreserveOriginalKeys
        {
            return m_table.keys();
        }

        void clear() noexcept
        {
            m_table.clear();
            m_values.clear();
            m_values.shrink_to_fit();
        }

        auto at(std::string_view key) -> T&
        {
            const auto index = index(key);
            NC_ASSERT(index != NullIndex, fmt::format("Key does not exist '{}'", key));
            return m_values[index];
        }

        auto at(std::string_view key) const -> const T&
        {
            const auto i = index(key);
            NC_ASSERT(i != NullIndex, fmt::format("Key does not exist '{}'", key));
            return m_values[i];
        }

        auto at(size_t index) -> T&
        {
            NC_ASSERT(index != NullIndex, fmt::format("Index does not exist '{}'", index));
            return m_values[index];
        }

        auto at(size_t index) const -> const T&
        {
            NC_ASSERT(index != NullIndex, fmt::format("Index does not exist '{}'", index));
            return m_values[index];
        }

        auto key_at(size_t hash) const -> std::string_view
        {
            return m_table.at(m_table.index(hash));
        }

        auto begin() noexcept { return m_values.begin(); }
        auto begin() const noexcept { return m_values.cbegin(); }
        auto end() noexcept { return m_values.end(); }
        auto end() const noexcept { return m_values.cend(); }
        auto empty() const noexcept -> bool { return m_values.empty(); }
        auto size() const noexcept -> size_t { return m_values.size(); }

    private:
        BasicStringTable<Policy> m_table;
        std::vector<T> m_values;
};
} // namespace nc
