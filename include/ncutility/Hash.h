/**
 * @file Hash.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <string_view>

namespace nc::utility
{
/** @cond internal */
namespace detail
{
constexpr size_t FnvOffsetBasis = 14695981039346656037ull;
constexpr size_t FnvPrime = 1099511628211ull;
}
/** @endcond internal */

/** @brief FNV1-a hash algorithm */
constexpr auto Fnv1a(std::string_view path) -> size_t
{
    auto hash = detail::FnvOffsetBasis;
    for(auto c : path)
    {
        hash = (hash ^ static_cast<uint8_t>(c)) * detail::FnvPrime;
    }
    return hash;
}

/** @brief A constexpr string hash wrapper */
class StringHash
{
    public:
        constexpr explicit StringHash(std::string_view path)
            : m_hash{Fnv1a(path)}
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
} // namespace nc::hash
