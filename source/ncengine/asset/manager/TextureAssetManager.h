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
        explicit TextureAssetManager(const std::string& texturesAssetDirectory,
                                     uint32_t maxTextures);

        auto Load(const std::string& path)                -> bool                          override;
        auto Load(std::span<const std::string> paths)     -> bool                          override;
        auto Unload(const std::string& path)              -> bool                          override;
        void UnloadAll()                                                                   override;
        auto Acquire(const std::string& path)       const -> TextureView                   override;
        auto Acquire(AssetId id)                    const -> TextureView                   override;
        auto GetAllLoaded()                         const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)      const -> bool                          override { return m_table.contains(path); }
        auto GetPath(AssetId id)                    const -> std::string_view              override { return m_table.at(m_table.index(id)); }
        auto GetAssetType()                         const -> AssetType                     override { return AssetType::Texture; }
        auto OnUpdate()                                   -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_table;
        std::string m_assetDirectory;
        uint32_t m_maxTextureCount;
        Signal<const TextureUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
