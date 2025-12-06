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
struct MaterialPassFlag
{
    using type = uint64_t;

    static constexpr auto UniShadow   = type{1 << 0};
    static constexpr auto PointShadow = type{1 << 1};
    static constexpr auto Depth       = type{1 << 2};
    static constexpr auto Toon        = type{1 << 3};
    static constexpr auto Normals     = type{1 << 4};
};

/** @brief Set of flags indicating a MaterialInstance's enabled passes. */
using MaterialPassFlags = MaterialPassFlag::type;

/** @brief Default passes for a toon material. */
constexpr auto ShadowedToonMaterial = MaterialPassFlag::UniShadow | MaterialPassFlag::PointShadow | MaterialPassFlag::Depth | MaterialPassFlag::Toon | MaterialPassFlag::Normals;

/** @brief Properties of a MaterialInstance passed to shaders. */
struct MaterialProperties
{
    Vector4 gradientStart = Vector4::One();
    Vector4 gradientEnd = Vector4::One();
    Vector4 primaryColor = Vector4::One();
    Vector4 secondaryColor = Vector4::One();
    Vector4 tertiaryColor = Vector4::One();
    float normalIntensity = 1.0f;
    float hatchTiling = 1.0f;
    float gradientAmount = 0.1f;
    float reflectivity = 0.0f;
    asset::TextureView diffuseTex = asset::TextureView{};
    asset::TextureView normalTex = asset::TextureView{};
    asset::TextureView hatchTex = asset::TextureView{};
    bool useTextureNormals = true;
    bool useFlatShading = true;
    bool useColorOverride = false;
    bool useHatchTexture = false;
};

/** @brief Properties for constructing a MaterialInstance. */
struct MaterialDesc
{
    std::string name = "DefaultMaterial";
    MaterialPassFlags passes = ShadowedToonMaterial;
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
        auto GetPasses() const -> MaterialPassFlags;

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
