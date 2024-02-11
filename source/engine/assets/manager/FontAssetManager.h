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
    auto operator()(const Font& font) const noexcept
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
    auto operator()(const Font& lhs, const Font& rhs) const noexcept
    {
        return lhs.size == rhs.size && lhs.path == rhs.path;
    }
};

class FontAssetManager : public IAssetService<FontView, Font>
{
    public:
        explicit FontAssetManager(const std::string& assetDirectory);

        bool Load(const Font& font, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const Font> fonts, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const Font& font, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const Font& font, asset_flags_type flags = AssetFlags::None) const -> FontView override;
        bool IsLoaded(const Font& font, asset_flags_type flags = AssetFlags::None) const override;
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto OnUpdate() -> Signal<>&;

        // TODO: doesn't exist
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::Font; }

    private:
        std::unordered_map<Font, FontView, FontHash, FontEqual> m_fonts;
        std::string m_assetDirectory;
        Signal<> m_onUpdate;
};
} // namespace nc
