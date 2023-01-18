#include "ShadowMapShaderResource.h"
#include "graphics/Initializers.h"
#include "graphics/PointLight.h"

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
    ShadowMapShaderResource::ShadowMapShaderResource(uint32_t bindingSlot, vk::Device device, Registry* registry, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, Vector2 dimensions)
    : m_device{device},
      m_allocator{allocator},
      m_descriptors{ descriptors },
      m_sampler{CreateShadowMapSampler(m_device)},
      m_depthStencils{},
      m_dimensions{dimensions},
      m_bindingSlot{bindingSlot},
      m_imageInfos{},
      m_onAddPointLightConnection{registry->OnAdd<PointLight>().Connect([this](graphics::PointLight&){ this->AddShadowMapResource(); }, 1u)},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){ this->RemoveShadowMapResource(); }, 1u)},
      m_numShadowCasters{0}
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
        
        for (uint32_t i = 0; i < m_numShadowCasters; ++i)
        {
            m_depthStencils.push_back(std::make_unique<RenderTarget>(m_device, m_allocator, m_dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
            m_imageInfos.push_back(CreateDescriptorImageInfo(m_sampler.get(), m_depthStencils.back()->GetImageView(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal));
          }

        m_descriptors->UpdateImage
        (   BindFrequency::per_frame,
            m_imageInfos,
            m_numShadowCasters,
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
            10,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
        );
    }

    void ShadowMapShaderResource::AddShadowMapResource()
    {
        m_numShadowCasters++;
        auto shadowMap = ShadowMap { .dimensions = m_dimensions };
        Update(std::vector<ShadowMap>{shadowMap});
    }

    void ShadowMapShaderResource::RemoveShadowMapResource()
    {
        m_numShadowCasters--;
        auto shadowMap = ShadowMap { .dimensions = m_dimensions };
        Update(std::vector<ShadowMap>{shadowMap});
    }
}