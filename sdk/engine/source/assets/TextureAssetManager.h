#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
    class TextureAssetManager : public IAssetService<TextureView, std::string>
    {
        public:
            TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures);
            ~TextureAssetManager() noexcept;

            bool Load(const std::string& path, bool isExternal) override;
            bool Load(std::span<const std::string> paths, bool isExternal) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> TextureView override;
            bool IsLoaded(const std::string& path) const override;
            auto OnUpdate() -> Signal<const TextureBufferData&>*;

        private:
            std::unordered_map<std::string, TextureView> m_accessors;
            std::string m_assetDirectory;
            uint32_t m_maxTextureCount;
            Signal<const TextureBufferData&> m_onUpdate;
    };
}