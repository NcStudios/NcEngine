#pragma once

#include "graphics/shaders/ShaderResourceService.h"
#include "graphics/shaders/ShaderDescriptorSets.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class TextureShaderResource : public IShaderResourceService<TextureBuffer>
{
    public:
        TextureShaderResource(uint32_t bindingSlot, ShaderDescriptorSets* descriptors, uint32_t maxTextures);
        ~TextureShaderResource() noexcept;

        void Initialize() override;
        void Update(const std::vector<TextureBuffer>& data) override;
        void Reset() override;

    private:
        ShaderDescriptorSets* m_descriptors;
        std::vector<vk::DescriptorImageInfo> m_imageInfos;
        vk::ImageLayout m_layout;
        uint32_t m_maxTexturesCount;
        bool m_texturesInitialized;
        uint32_t m_bindingSlot;
};
} // namespace nc::graphics