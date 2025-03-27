#pragma once

#include "asset/AssetService.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc::asset
{
struct FontHash
{
    auto operator()(const FontInfo& font) const noexcept
    {
        // roughly Boost hash_combine
        auto hash = 17ull;
        hash ^= std::hash<std::string>{}(font.path) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>{}(font.size) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
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

        auto Load(const FontInfo& font,
                  AssetSubtype = AssetSubtype::None) -> bool                          override;
        auto Load(std::span<const FontInfo> fonts,
                  AssetSubtype = AssetSubtype::None) -> bool                          override;
        auto Unload(const FontInfo& font)            -> bool                          override;
        void UnloadAll()                                                              override;
        auto Acquire(const FontInfo& font)     const -> FontView                      override;
        auto Acquire(AssetId)                  const -> FontView                      override { throw NcError{"Not Implemented"};}
        auto GetAllLoaded()                    const -> std::vector<std::string_view> override;
        auto IsLoaded(const FontInfo& font)    const -> bool                          override;
        auto GetPath(AssetId)                  const -> std::string_view              override { throw NcError{"Not Implemented"}; }
        auto GetAssetType()                    const -> asset::AssetType              override { return asset::AssetType::Font; }
        auto OnUpdate()                              -> decltype(auto)                         { return (m_onUpdate); }

    private:
        std::unordered_map<FontInfo, FontView, FontHash, FontEqual> m_fonts;
        std::string m_assetDirectory;
        Signal<> m_onUpdate;
};
} // namespace nc::asset
