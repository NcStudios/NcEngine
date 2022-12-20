#include "ShadowMapShaderResource.h"
#include "graphics/Initializers.h"

#include <vector>

namespace nc::graphics
{
    ShadowMapShaderResource::ShadowMapShaderResource(vk::Device device, uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, Vector2 dimensions, uint32_t numLights)
    : m_device{device},
      m_allocator{allocator},
      m_descriptors{ descriptors },
      m_numLights{numLights},
      m_samplers{},
      m_depthStencils{},
      m_dimensions{dimensions},
      m_bindingSlot{bindingSlot},
      m_imageInfos{},
      m_isRegistered{false}
    {
        Initialize();
    }

    ShadowMapShaderResource::~ShadowMapShaderResource()
    {
        Reset();
    }

    void ShadowMapShaderResource::Reset()
    {
    }

    void ShadowMapShaderResource::Update(const std::vector<ShadowMap>& data)
    {
        m_dimensions = data.at(0).dimensions;

        m_depthStencils.clear();
        m_imageInfos.clear();
        for (uint32_t i = 0; i < m_numLights; ++i)
        {
            m_depthStencils.push_back(std::make_unique<RenderTarget>(m_device, m_allocator, m_dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
            m_imageInfos.push_back(CreateDescriptorImageInfo(m_samplers[i].get(), m_depthStencils.back()->GetImageView(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal));
          }

        m_descriptors->UpdateImage
        (
            BindFrequency::per_frame,
            m_imageInfos,
            m_numLights,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }

    void ShadowMapShaderResource::Initialize()
    {
        // Create sampler which will be used to sample in the fragment shader to get shadow data.
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

        m_depthStencils.clear();
        m_imageInfos.clear();
        for (uint32_t i = 0; i < m_numLights; ++i)
        {
            m_samplers.push_back(m_device.createSamplerUnique(samplerInfo));
            m_depthStencils.push_back(std::make_unique<RenderTarget>(m_device, m_allocator, m_dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
            m_imageInfos.push_back(CreateDescriptorImageInfo(m_samplers[i].get(), m_depthStencils.back()->GetImageView(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal));
        }
        
        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::per_frame,
            m_numLights,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT()
        );
        m_isRegistered = true;

        m_descriptors->UpdateImage
        (
            BindFrequency::per_frame,
            m_imageInfos,
            m_numLights,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }
}