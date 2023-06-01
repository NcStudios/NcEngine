#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <string>
#include <vector>

namespace nc
{
namespace asset
{
struct TextureWithId;
struct TextureUpdateEventData;
} // namespace nc

class TextureAssetManager : public IAssetService<TextureView, std::string>
{
    public:
        explicit TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures);
        ~TextureAssetManager() noexcept;

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> TextureView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto OnUpdate() -> Signal<const asset::TextureUpdateEventData&>&;

    private:
        std::vector<asset::TextureWithId> m_textureData;
        std::string m_assetDirectory;
        uint32_t m_maxTextureCount;
        Signal<const asset::TextureUpdateEventData&> m_onUpdate;
};
} // namespace nc
