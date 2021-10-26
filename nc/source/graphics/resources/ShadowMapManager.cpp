#include "ShadowMapManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ShadowMapManager::ShadowMapManager(Graphics* graphics, Vector2 dimensions)
    : m_graphics{graphics},
      m_sampler{nullptr}, 
      m_depthStencil{nullptr},
      m_descriptorSet{nullptr},
      m_descriptorSetLayout{nullptr},
      m_dimensions{dimensions}
    {
        Initialize();
    }

    ShadowMapManager::~ShadowMapManager()
    {
        Reset();
    }

    auto ShadowMapManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto ShadowMapManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void ShadowMapManager::Reset()
    {
        // m_sampler.reset();
        // m_depthStencil.reset();
        // m_descriptorSet.reset();
        // m_descriptorSetLayout.reset();
        // Initialize();
    }

    void ShadowMapManager::Update(const std::vector<ShadowMap>& data)
    {
        m_dimensions = data.at(0).dimensions;
        auto* base = m_graphics->GetBasePtr();

        m_depthStencil.reset();
        m_depthStencil = std::make_unique<DepthStencil>(base, m_dimensions, vk::Format::eD16Unorm);
        
        vk::DescriptorImageInfo imageSamplerInfo;
        imageSamplerInfo.setSampler(m_sampler.get());
        imageSamplerInfo.setImageView(m_depthStencil->GetImageView());
        imageSamplerInfo.setImageLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(0);
        write.setPImageInfo(&imageSamplerInfo);
 
        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void ShadowMapManager::Initialize()
    {
        auto* base = m_graphics->GetBasePtr();

        m_depthStencil = std::make_unique<DepthStencil>(base, m_dimensions, vk::Format::eD16Unorm);

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
        
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = 
        {
            CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment )
        };

        std::array<vk::DescriptorBindingFlagsEXT, 1> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT()
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_descriptorSet = CreateDescriptorSet(m_graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

        vk::DescriptorImageInfo imageSamplerInfo;
        imageSamplerInfo.setSampler(m_sampler.get());
        imageSamplerInfo.setImageView(m_depthStencil->GetImageView());
        imageSamplerInfo.setImageLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(0);
        write.setPImageInfo(&imageSamplerInfo);
 
        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }
}