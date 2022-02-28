#pragma once

#include "ShaderResourceService.h"
#include "ImmutableImage.h"
#include "graphics/resources/shader_descriptor_sets.h"

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
            TextureManager(uint32_t bindingSlot, Graphics* graphics, shader_descriptor_sets* descriptors, uint32_t maxTextures);
            ~TextureManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<Texture>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            shader_descriptor_sets* m_descriptors;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::ImageLayout m_layout;
            uint32_t m_maxTexturesCount;
            bool m_texturesInitialized;
            uint32_t m_bindingSlot;
    };
}