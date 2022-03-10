#include "TextureManager.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    TextureManager::TextureManager(uint32_t bindingSlot, Graphics* graphics, shader_descriptor_sets* descriptors, uint32_t maxTextures)
        : m_graphics{graphics},
          m_descriptors{descriptors},
          m_imageInfos{},
          m_layout{},
          m_maxTexturesCount{maxTextures},
          m_texturesInitialized{false},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    TextureManager::~TextureManager() noexcept
    {
    }

    void TextureManager::Initialize()
    {
        m_descriptors->register_descriptor
        (
            m_bindingSlot,
            bind_frequency::per_frame,
            m_maxTexturesCount,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
        );
    }

    void TextureManager::Update(const std::vector<Texture>& data)
    {
        assert(data.size() < m_maxTexturesCount && !data.empty());

        if (!m_texturesInitialized)
        {
            m_imageInfos = std::vector<vk::DescriptorImageInfo>(m_maxTexturesCount, data.at(0).imageInfo);
            m_texturesInitialized = true;
        }

        std::transform(data.cbegin(), data.cend(), m_imageInfos.begin(), [](const auto& texture)
        {
            return texture.imageInfo;
        });

        m_descriptors->update_image
        (
            bind_frequency::per_frame,
            m_imageInfos,
            m_maxTexturesCount,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }

    void TextureManager::Reset()
    {
    }
}