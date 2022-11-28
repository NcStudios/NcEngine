#pragma once

#include "ncutility/Hash.h"

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

template<AssetType T>
constexpr bool operator==(AssetId<T> lhs, AssetId<T> rhs)
{
    return lhs.Id() == rhs.Id();
}

template<AssetType T>
constexpr bool operator!=(AssetId<T> lhs, AssetId<T> rhs)
{
    return !(lhs == rhs);
}
} // namespace nc
