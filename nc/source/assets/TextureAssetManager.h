#pragma once

#include "assets/AssetService.h"

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace nc
{
    namespace graphics
    {
        class Graphics;
        struct Texture;
    }

    class TextureAssetManager : public IAssetService<TextureView>
    {
        public:
            TextureAssetManager(graphics::Graphics* graphics, uint32_t maxTextures);
            ~TextureAssetManager() noexcept;

            bool Load(const std::string& path) override;
            bool Load(const std::vector<std::string>& paths) override;
            bool Unload(const std::string& path) override;
            auto Acquire(const std::string& path) const -> TextureView override;
            bool IsLoaded(const std::string& path) const override;

        private:
            std::unordered_map<std::string, TextureView> m_accessors;
            std::vector<graphics::Texture> m_textures;
            graphics::Graphics* m_graphics;
            vk::UniqueSampler m_sampler;
            uint32_t m_maxTextureCount;
    };
}