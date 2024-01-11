#include "ShadowMapShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"

#include <vector>

namespace 
{
vk::UniqueSampler CreateShadowMapSampler(vk::Device device)
{
    constexpr auto samplerInfo = vk::SamplerCreateInfo
    {
        vk::SamplerCreateFlags(),               // SamplerCreateFlags
        vk::Filter::eNearest,                   // MagFilter
        vk::Filter::eNearest,                   // MinFilter
        vk::SamplerMipmapMode::eLinear,         // MipMapMode
        vk::SamplerAddressMode::eClampToBorder, // AddressModeU
        vk::SamplerAddressMode::eClampToBorder, // AddressModeV
        vk::SamplerAddressMode::eClampToBorder, // AddressModeW
        0.0f,                                   // MipLodBias
        VK_TRUE,                                // AnisotropyEnable
        1.0f,                                   // MaxAnisotropy
        VK_FALSE,                               // CompareEnable
        vk::CompareOp::eAlways,                 // CompareOp
        0.0f,                                   // MinLod
        1.0f,                                   // MaxLod
        vk::BorderColor::eFloatOpaqueWhite,     // BorderColor
        VK_FALSE                                // UnnormalizedCoordinates
    };
    return device.createSamplerUnique(samplerInfo);
}
}
namespace nc::graphics
{
    ShadowMapShaderResource::ShadowMapShaderResource(uint32_t bindingSlot, vk::Device device, ShaderDescriptorSets* descriptors, uint32_t maxShadows)
    : m_descriptors{ descriptors },
      m_sampler{CreateShadowMapSampler(device)},
      m_bindingSlot{bindingSlot},
      m_maxShadows{maxShadows}
    {
        Initialize();
    }

    void ShadowMapShaderResource::Reset()
    {
        m_imageInfos.clear();
    }

    void ShadowMapShaderResource::Update(const std::vector<ShadowMap>& data)
    {
        assert(data.size() < m_maxShadows && !data.empty());

        m_imageInfos.clear();
        m_imageInfos.reserve(m_maxShadows);

        std::ranges::transform(data, std::back_inserter(m_imageInfos), [this](auto &dataItem)
        {
            return CreateDescriptorImageInfo(m_sampler.get(), dataItem.imageView, vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
        });

        // Fill the rest of the buffer with default values to satisfy VK_CmdDrawIndexed_None_02669
        std::generate_n(std::back_inserter(m_imageInfos), m_maxShadows - static_cast<uint32_t>(m_imageInfos.size()), [&first = m_imageInfos.at(0)]() -> vk::DescriptorImageInfo& { return first; });

        m_descriptors->UpdateImage
        (   
            BindFrequency::per_frame,
            m_imageInfos,
            static_cast<uint32_t>(m_imageInfos.size()),
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }

    void ShadowMapShaderResource::Initialize()
    {
        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::per_frame,
            m_maxShadows,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT()
        );
    }
}