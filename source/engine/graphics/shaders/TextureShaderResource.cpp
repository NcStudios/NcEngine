#include "TextureShaderResource.h"
#include "graphics/Initializers.h"
#include "graphics/textures/ImmutableImage.h"
#include "graphics/textures/Textures.h"
#include "utility/NcError.h"

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
        vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
    );
}

void TextureShaderResource::Update(const std::vector<TextureBuffer>& data)
{
    assert(data.size() < m_maxTexturesCount && !data.empty());

    m_imageInfos = std::vector<vk::DescriptorImageInfo>();
    
    std::transform(data.cbegin(), data.cend(), std::back_inserter(m_imageInfos), [](const auto& texture)
    {
        return texture.imageInfo;
    });

    m_descriptors->UpdateImage
    (
        BindFrequency::per_frame,
        m_imageInfos,
        static_cast<uint32_t>(data.size()),
        vk::DescriptorType::eCombinedImageSampler,
        m_bindingSlot
    );
}

void TextureShaderResource::Reset()
{
}
} // namespace nc::graphics