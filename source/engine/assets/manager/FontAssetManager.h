#pragma once

#include "assets/AssetService.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc
{
struct FontHash
{
    auto operator()(const FontInfo& font) const noexcept
    {
        auto stringHasher = std::hash<std::string>{};
        auto floatHasher = std::hash<float>{};
        auto hash = 17ull;
        hash = hash * 31 + stringHasher(font.path);
        hash = hash * 31 + floatHasher(font.size);
        return hash;
    }
};

struct FontEqual
{
    auto operator()(const FontInfo& lhs, const FontInfo& rhs) const noexcept
    {
        return lhs.size == rhs.size && lhs.path == rhs.path;
    }
};

class FontAssetManager : public IAssetService<FontView, FontInfo>
{
    public:
        explicit FontAssetManager(const std::string& assetDirectory);

        bool Load(const FontInfo& font, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const FontInfo> fonts, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const FontInfo& font, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const FontInfo& font, asset_flags_type flags = AssetFlags::None) const -> FontView override;
        bool IsLoaded(const FontInfo& font, asset_flags_type flags = AssetFlags::None) const override;
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto OnUpdate() -> Signal<>&;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::Font; }

    private:
        std::unordered_map<FontInfo, FontView, FontHash, FontEqual> m_fonts;
        std::string m_assetDirectory;
        Signal<> m_onUpdate;
};
} // namespace nc
