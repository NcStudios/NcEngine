#include "ShadowMapManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

#include <vector>

namespace nc::graphics
{
    ShadowMapManager::ShadowMapManager(uint32_t bindingSlot, Graphics* graphics, shader_descriptor_sets* descriptors, Vector2 dimensions)
    : m_graphics{graphics},
      m_descriptors{ descriptors },
      m_sampler{nullptr},
      m_depthStencil{nullptr},
      m_dimensions{dimensions},
      m_bindingSlot{bindingSlot},
      m_imageInfos{},
      m_isRegistered{false}
    {
        Initialize();
    }

    ShadowMapManager::~ShadowMapManager()
    {
        Reset();
    }

    void ShadowMapManager::Reset()
    {
    }

    void ShadowMapManager::Update(const std::vector<ShadowMap>& data)
    {
        m_dimensions = data.at(0).dimensions;
        auto* base = m_graphics->GetBasePtr();

        m_depthStencil.reset();
        m_depthStencil = std::make_unique<RenderTarget>(base, m_dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm);

        auto descriptorImageInfo = CreateDescriptorImageInfo(&m_sampler.get(), m_depthStencil->GetImageView(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
        m_imageInfos = std::vector<vk::DescriptorImageInfo>(1, descriptorImageInfo);

        m_descriptors->update_image
        (
            bind_frequency::per_frame,
            m_imageInfos,
            1,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }

    void ShadowMapManager::Initialize()
    {
        auto* base = m_graphics->GetBasePtr();

        m_depthStencil.reset();
        m_depthStencil = std::make_unique<RenderTarget>(base, m_dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm);

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

        m_sampler = base->GetDevice().createSamplerUnique(samplerInfo);

        auto descriptorImageInfo = CreateDescriptorImageInfo(&m_sampler.get(), m_depthStencil->GetImageView(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
        m_imageInfos = std::vector<vk::DescriptorImageInfo>(1, descriptorImageInfo);

        m_descriptors->register_descriptor
        (
            m_bindingSlot,
            bind_frequency::per_frame,
            1,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT()
        );
        m_isRegistered = true;

        m_descriptors->update_image
        (
            bind_frequency::per_frame,
            m_imageInfos,
            1,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }
}