#pragma once

#include "utility/Hash.h"

namespace nc
{
enum class AssetType
{
    AudioClip,
    ConcaveCollider,
    HullCollider,
    Mesh,
    Shader,
    Skybox,
    Texture
};

template<AssetType T>
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

template<AssetType T>
constexpr bool operator==(AssetId<T> lhs, AssetId<T> rhs)
{
    return lhs.Hash() == rhs.Hash();
}

template<AssetType T>
constexpr bool operator!=(AssetId<T> lhs, AssetId<T> rhs)
{
    return !(lhs == rhs);
}
} // namespace nc