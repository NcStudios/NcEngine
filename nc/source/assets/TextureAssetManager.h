#pragma once

#include "assets/AssetService.h"
#include "graphics/resources/CubeMap.h"

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

    class TextureAssetManager : public IAssetService<TextureView>
    {
        public:
            TextureAssetManager(graphics::Graphics* graphics, uint32_t maxTextures);
            ~TextureAssetManager() noexcept;

            bool LoadCubeMap(const std::string& frontPath,
                             const std::string& backPath,
                             const std::string& upPath,
                             const std::string& downPath,
                             const std::string& rightPath,
                             const std::string& leftPath);
            bool Load(const std::string& path) override;
            bool Load(std::span<const std::string> paths) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> TextureView override;
            bool IsLoaded(const std::string& path) const override;

        private:
            std::unordered_map<std::string, TextureView> m_textureAccessors;
            std::vector<graphics::Texture> m_textures;
            std::vector<graphics::CubeMap> m_cubeMaps;
            graphics::Graphics* m_graphics;
            vk::UniqueSampler m_textureSampler;
            vk::UniqueSampler m_cubeMapSampler;
            uint32_t m_maxTextureCount;
    };
}