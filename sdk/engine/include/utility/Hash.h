#pragma once

#include <string_view>

namespace nc::hash
{
constexpr size_t FnvOffsetBasis = 14695981039346656037ull;
constexpr size_t FnvPrime = 1099511628211ull;

constexpr auto Fnv1a(std::string_view path) -> size_t
{
    auto hash = FnvOffsetBasis;
    for(auto c : path)
    {
        hash = (hash ^ c) * FnvPrime;
    }
    return hash;
}

class StringHash
{
    public:
        constexpr explicit StringHash(std::string_view path)
            : m_hash{hash::Fnv1a(path)}
        {
        }

        constexpr auto Hash() const noexcept -> size_t
        {
            return m_hash;
        }

    private:
        size_t m_hash;
};

constexpr bool operator==(StringHash lhs, StringHash rhs)
{
    return lhs.Hash() == rhs.Hash();
}

constexpr bool operator!=(StringHash lhs, StringHash rhs)
{
    return !(lhs == rhs);
}
}