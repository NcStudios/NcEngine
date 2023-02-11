#pragma once

#include "ncutility/Hash.h"

#include "ncasset/AssetType.h"

namespace nc
{
template<asset::AssetType T>
class AssetId
{
    public:
        constexpr explicit AssetId(std::string_view path)
            : m_hash{path}
        {
        }

        constexpr auto Id() const noexcept -> size_t
        {
            return m_hash.Hash();
        }

    private:
        utility::StringHash m_hash;
};

template<asset::AssetType T>
constexpr bool operator==(AssetId<T> lhs, AssetId<T> rhs)
{
    return lhs.Id() == rhs.Id();
}

template<asset::AssetType T>
constexpr bool operator!=(AssetId<T> lhs, AssetId<T> rhs)
{
    return !(lhs == rhs);
}
} // namespace nc
