#pragma once

#include "ShaderResourceService.h"
#include "ImmutableImage.h"
#include "graphics/resources/ShaderDescriptorSets.h"
#include "graphics/GpuAssetsData.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class TextureManager : public IShaderResourceService<TextureImageInfo>
    {
        public:
            TextureManager(uint32_t bindingSlot, Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxTextures);
            ~TextureManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<TextureImageInfo>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            ShaderDescriptorSets* m_descriptors;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            vk::ImageLayout m_layout;
            uint32_t m_maxTexturesCount;
            bool m_texturesInitialized;
            uint32_t m_bindingSlot;
    };
}