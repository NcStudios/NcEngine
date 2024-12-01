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
using MaterialInstanceHandle = uint32_t;

/** @brief Null identifier for a MaterialInstance. */
constexpr auto NullMaterialInstanceHandle = std::numeric_limits<MaterialInstanceHandle>::max();

/** @brief Material pass flags. */
struct MaterialPass
{
    using type = uint64_t;

    static constexpr auto Shadow  = type{1 << 0};
    static constexpr auto Toon    = type{1 << 1};
};

/** @brief Set of flags indicating a MaterialInstance's enabled passes. */
using MaterialPasses = MaterialPass::type;

/** @brief Default passes for a toon material. */
constexpr auto ShadowedToonMaterial = MaterialPass::Shadow | MaterialPass::Toon;

/** @brief Properties of a MaterialInstance passed to shaders. */
struct MaterialProperties
{
    asset::TextureView diffuseTexture = asset::TextureView{};
    asset::TextureView normalTexture = asset::TextureView{};
    Vector3 gradientStart = Vector3::One();
    Vector3 gradientEnd = Vector3::One();
    Vector3 outlineColor = Vector3::Zero();
    float outlineWidth = 1.0f;
};

/** @brief Properties for constructing a MaterialInstance. */
struct MaterialDesc
{
    std::string name = "DefaultMaterial";
    MaterialPasses passes = ShadowedToonMaterial;
    MaterialProperties properties = MaterialProperties{};
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

        /** @name Name Functions */
        auto GetName() const -> std::string_view;
        void SetName(std::string_view name);

        /** @name MaterialPass Functions */
        auto GetPasses() const -> MaterialPasses;

        /** @name MaterialProperties Functions */
        auto GetProperties() const -> const MaterialProperties&;
        void SetProperties(const MaterialProperties& properties);

        /** @brief Get the instance's handle. */
        auto GetHandle() const -> MaterialInstanceHandle
        {
            return m_handle;
        }

    private:
        friend class graphics::MaterialRegistry;
        static inline graphics::MaterialRegistry* s_registry = nullptr;

        MaterialInstanceHandle m_handle;

        void Release() noexcept;
};

inline MaterialInstance::MaterialInstance(MaterialInstance&& other) noexcept
    : m_handle{std::exchange(other.m_handle, NullMaterialInstanceHandle)}
{
}

inline MaterialInstance& MaterialInstance::operator=(MaterialInstance&& other) noexcept
{
    if (this != &other)
    {
        Release();
        m_handle = std::exchange(other.m_handle, NullMaterialInstanceHandle);
    }

    return *this;
}

inline MaterialInstance::~MaterialInstance() noexcept
{
    Release();
}
} // namespace nc
