#pragma once

#include "ShaderResourceService.h"
#include "ImmutableImage.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    struct Texture
    {
        ImmutableImage image;
        vk::DescriptorImageInfo imageInfo;
        std::string uid;
    };

    class TextureManager : public IShaderResourceService<Texture>
    {
        public:
            TextureManager(Graphics* graphics, uint32_t maxTextures);
            ~TextureManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<Texture>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            vk::UniqueSampler m_textureSampler;
            vk::UniqueSampler m_cubeMapSampler;
            vk::ImageLayout m_layout;
            uint32_t m_maxTexturesCount;
            bool m_texturesInitialized;
    };
}