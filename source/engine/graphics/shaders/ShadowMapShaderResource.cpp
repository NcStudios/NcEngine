#include "ShadowMapShaderResource.h"
#include "graphics/Initializers.h"

#include <vector>

namespace 
{
vk::UniqueSampler CreateShadowMapSampler(vk::Device device)
{
    vk::SamplerCreateInfo samplerInfo = {};
    samplerInfo.setMagFilter(vk::Filter::eNearest);
    samplerInfo.setMinFilter(vk::Filter::eNearest);
    samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    samplerInfo.setAddressModeU(vk::SamplerAddressMode::eClampToBorder);
    samplerInfo.setAddressModeV(vk::SamplerAddressMode::eClampToBorder);
    samplerInfo.setAddressModeW(vk::SamplerAddressMode::eClampToBorder);
    samplerInfo.setAnisotropyEnable(VK_TRUE);
    samplerInfo.setMaxAnisotropy(1.0f);
    samplerInfo.setBorderColor(vk::BorderColor::eFloatOpaqueWhite);
    samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
    samplerInfo.setCompareEnable(VK_FALSE);
    samplerInfo.setCompareOp(vk::CompareOp::eAlways);
    samplerInfo.setMipLodBias(0.0f);
    samplerInfo.setMinLod(0.0f);
    samplerInfo.setMaxLod(1.0f);

    return device.createSamplerUnique(samplerInfo);
}
}
namespace nc::graphics
{
    ShadowMapShaderResource::ShadowMapShaderResource(uint32_t bindingSlot, vk::Device device, ShaderDescriptorSets* descriptors, uint32_t maxShadows)
    : m_descriptors{ descriptors },
      m_sampler{CreateShadowMapSampler(device)},
      m_imageInfos{},
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
        m_imageInfos.clear();
        m_imageInfos.reserve(data.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(data.size()); ++i)
        {
            m_imageInfos.push_back(CreateDescriptorImageInfo(m_sampler.get(), data[i].imageView, vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal));
        }

        m_descriptors->UpdateImage
        (   BindFrequency::per_frame,
            m_imageInfos,
            static_cast<uint32_t>(data.size()),
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
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
        );
    }
}