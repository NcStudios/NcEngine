#pragma once

#include "assets/AssetService.h"

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
    namespace graphics
    {
        class Graphics;
        struct Texture;
    }

    class TextureAssetManager : public IAssetService<TextureView, std::string>
    {
        public:
            TextureAssetManager(graphics::Graphics* graphics, uint32_t maxTextures);
            ~TextureAssetManager() noexcept;

            bool Load(const std::string& path) override;
            bool Load(std::span<const std::string> paths) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> TextureView override;
            bool IsLoaded(const std::string& path) const override;

        private:
            std::unordered_map<std::string, TextureView> m_textureAccessors;
            std::vector<graphics::Texture> m_textures;
            graphics::Graphics* m_graphics;
            vk::UniqueSampler m_textureSampler;
            uint32_t m_maxTextureCount;
    };
}