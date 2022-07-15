#pragma once

#include "utility/Hash.h"

namespace nc
{
enum class Asset_t : uint8_t
{
    AudioClip,
    ConcaveCollider,
    ConvexCollider,
    CubeMap,
    Mesh,
    Shader,
    Texture
};

template<Asset_t T>
class AssetId
{
    public:
        constexpr explicit AssetId(std::string_view path)
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

template<Asset_t T>
constexpr bool operator==(AssetId<T> lhs, AssetId<T> rhs)
{
    return lhs.Hash() == rhs.Hash();
}

template<Asset_t T>
constexpr bool operator!=(AssetId<T> lhs, AssetId<T> rhs)
{
    return !(lhs == rhs);
}
} // namespace nc