#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <string>
#include <vector>

namespace nc
{
struct TextureBufferData;
struct TextureData;

class TextureAssetManager : public IAssetService<TextureView, std::string>
{
    public:
        explicit TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures);

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> TextureView override;
        bool IsLoaded(const std::string& path) const override;
        auto OnUpdate() -> Signal<const TextureBufferData&>&;

    private:
        std::vector<TextureData> m_textureData;
        std::string m_assetDirectory;
        uint32_t m_maxTextureCount;
        Signal<const TextureBufferData&> m_onUpdate;
};
} // namespace nc
