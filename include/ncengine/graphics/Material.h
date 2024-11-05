/**
 * @file Material.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncmath/Vector.h"

#include <limits>
#include <utility>

namespace nc
{
namespace graphics
{
class MaterialRegistry;
} // namespace graphics

/** @brief Identifier for a MaterialInstance. */
using MaterialInstanceHandle = uint16_t;

/** @brief Null identifier for a MaterialInstance. */
constexpr auto NullMaterialInstanceHandle = std::numeric_limits<MaterialInstanceHandle>::max();

/** @brief Set of flags indicating a MaterialInstance's enabled passes. */
using MaterialPasses = uint64_t;

/** @brief Material pass flags. */
struct MaterialPass
{
    static constexpr auto Shadow  = MaterialPasses{1 << 0};
    static constexpr auto Toon    = MaterialPasses{1 << 1};
    static constexpr auto Alpha   = MaterialPasses{1 << 2};
    static constexpr auto Depth   = MaterialPasses{1 << 3};
    static constexpr auto Normals = MaterialPasses{1 << 4};
    static constexpr auto Outline = MaterialPasses{1 << 5};
};

/** @brief Default passes for a toon material. */
constexpr auto ShadowedToonMaterial = MaterialPass::Shadow |
                                      MaterialPass::Toon |
                                      MaterialPass::Alpha |
                                      MaterialPass::Depth |
                                      MaterialPass::Normals |
                                      MaterialPass::Outline;

/** @brief Properties for constructing a MaterialInstance. */
struct MaterialDesc
{
    std::string name = "DefaultMaterial";
    MaterialPasses passes = ShadowedToonMaterial;
    asset::TextureView diffuseTexture = asset::TextureView{};
    asset::TextureView normalTexture = asset::TextureView{};
    Vector3 gradientStart = Vector3::One();
    Vector3 gradientEnd = Vector3::One();
    Vector3 outlineColor = Vector3::One();
    float outlineWidth = 1.0f;
};

/** @brief Owning wrapper around a material in GPU memory. */
class MaterialInstance
{
    public:
        explicit MaterialInstance(const MaterialDesc& desc = MaterialDesc{});
        MaterialInstance(MaterialInstance&& other) noexcept;
        MaterialInstance& operator=(MaterialInstance&& other) noexcept;
        ~MaterialInstance() noexcept;

        MaterialInstance(const MaterialInstance&) = delete;
        MaterialInstance& operator=(const MaterialInstance&) = delete;

        /** @brief Create a new MaterialInstance from this instance's properties. */
        auto Clone() const -> MaterialInstance;

        /** @brief Get the instance's properties. */
        auto GetDesc() const -> const MaterialDesc&;

        /** @brief Update the instance's properties. */
        void SetDesc(const MaterialDesc& desc);

        /** @brief Get the instance's handle. */
        auto GetHandle() const -> MaterialInstanceHandle
        {
            return m_handle;
        }

    private:
        friend class graphics::MaterialRegistry;
        static inline graphics::MaterialRegistry* s_registry = nullptr;

        MaterialInstanceHandle m_handle;
};

inline MaterialInstance::MaterialInstance(MaterialInstance&& other) noexcept
    : m_handle{std::exchange(other.m_handle, NullMaterialInstanceHandle)}
{
}

inline MaterialInstance& MaterialInstance::operator=(MaterialInstance&& other) noexcept
{
    if (this != &other)
    {
        m_handle = std::exchange(other.m_handle, NullMaterialInstanceHandle);
    }

    return *this;
}
} // namespace nc
