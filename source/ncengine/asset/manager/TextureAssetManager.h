#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include <string>

namespace nc::asset
{
struct TextureWithId;
struct TextureUpdateEventData;

class TextureAssetManager : public IAssetService<TextureView, std::string>
{
    public:
        explicit TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> TextureView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetPath(size_t id) const -> std::string_view override { return m_table.at(m_table.index(id)); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> AssetType override { return AssetType::Texture; }
        auto OnUpdate() -> Signal<const TextureUpdateEventData&>&;

    private:
        StringTable m_table;
        std::string m_assetDirectory;
        uint32_t m_maxTextureCount;
        Signal<const TextureUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
