#include "TextureShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/textures/TextureBuffer.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
TextureShaderResource::TextureShaderResource(uint32_t bindingSlot, ShaderDescriptorSets* descriptors, uint32_t maxTextures)
    : m_descriptors{descriptors},
        m_imageInfos{},
        m_layout{},
        m_maxTexturesCount{maxTextures},
        m_texturesInitialized{false},
        m_bindingSlot{bindingSlot}
{
    Initialize();
}

TextureShaderResource::~TextureShaderResource() noexcept
{
}

void TextureShaderResource::Initialize()
{
    m_descriptors->RegisterDescriptor
    (
        m_bindingSlot,
        BindFrequency::per_frame,
        m_maxTexturesCount,
        vk::DescriptorType::eCombinedImageSampler,
        vk::ShaderStageFlagBits::eFragment,
        vk::DescriptorBindingFlagBitsEXT()
    );
}

void TextureShaderResource::Update(const std::vector<TextureBuffer>& data)
{
    assert(data.size() < m_maxTexturesCount && !data.empty());

    m_imageInfos = std::vector<vk::DescriptorImageInfo>();
    m_imageInfos.reserve(m_maxTexturesCount);
    
    std::transform(data.cbegin(), data.cend(), std::back_inserter(m_imageInfos), [](const auto& texture)
    {
        return texture.imageInfo;
    });

    // Fill the rest of the buffer with default values to satisfy VK_CmdDrawIndexed_None_02669
    std::generate_n(std::back_inserter(m_imageInfos), m_maxTexturesCount - static_cast<uint32_t>(m_imageInfos.size()), [&first = m_imageInfos.at(0)]() -> vk::DescriptorImageInfo& { return first; });

    m_descriptors->UpdateImage
    (
        BindFrequency::per_frame,
        m_imageInfos,
        static_cast<uint32_t>(m_imageInfos.size()),
        vk::DescriptorType::eCombinedImageSampler,
        m_bindingSlot
    );
}

void TextureShaderResource::Reset()
{
}
} // namespace nc::graphics