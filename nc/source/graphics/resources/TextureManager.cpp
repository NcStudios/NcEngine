#include "TextureManager.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    TextureManager::TextureManager(Graphics* graphics, uint32_t maxTextures)
        : m_graphics{graphics},
          m_imageInfos{},
          m_descriptorSet{},
          m_descriptorSetLayout{},
          m_textureSampler{},
          m_cubeMapSampler{},
          m_layout{},
          m_maxTexturesCount{maxTextures},
          m_texturesInitialized{false}
    {
        Initialize();
    }

    TextureManager::~TextureManager() noexcept
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        m_textureSampler.reset();
        m_cubeMapSampler.reset();
    }

    void TextureManager::Initialize()
    {
        // Create and bind the descriptor set for the array of textures.
        std::array<vk::DescriptorSetLayoutBinding, 2u> layoutBindings
        {
          CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eSampler, vk::ShaderStageFlagBits::eFragment),
          CreateDescriptorSetLayoutBinding(1, m_maxTexturesCount, vk::DescriptorType::eSampledImage, vk::ShaderStageFlagBits::eFragment)
        };

        std::array<vk::DescriptorBindingFlagsEXT, 2> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT(),
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound 
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_descriptorSet = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

        /** @todo Figure out why there are validation errors when creating sampler in the constructor. */
        m_textureSampler = m_graphics->GetBasePtr()->CreateTextureSampler();
        m_cubeMapSampler = m_graphics->GetBasePtr()->CreateTextureSampler();
    }

    void TextureManager::Update(const std::vector<Texture>& data)
    {
        assert(data.size() < m_maxTexturesCount && !data.empty());

        std::array<vk::WriteDescriptorSet, 2> writes;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = m_textureSampler.get();

        if (!m_texturesInitialized)
        {
            m_imageInfos = std::vector<vk::DescriptorImageInfo>(m_maxTexturesCount, data.at(0).imageInfo);
            m_texturesInitialized = true;
        }

        writes[0].setDstBinding(0);
        writes[0].setDstArrayElement(0);
        writes[0].setDescriptorType(vk::DescriptorType::eSampler);
        writes[0].setDescriptorCount(1);
        writes[0].setDstSet(m_descriptorSet.get());
        writes[0].setPBufferInfo(0);
        writes[0].setPImageInfo(&samplerInfo);

        std::transform(data.cbegin(), data.cend(), m_imageInfos.begin(), [](const auto& texture)
        {
            return texture.imageInfo;
        });

        writes[1].setDstBinding(1);
        writes[1].setDstArrayElement(0);
        writes[1].setDescriptorType(vk::DescriptorType::eSampledImage);
        writes[1].setDescriptorCount(m_maxTexturesCount);
        writes[1].setDstSet(m_descriptorSet.get());
        writes[1].setPBufferInfo(0);
        writes[1].setPImageInfo(m_imageInfos.data());

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
    }

    auto TextureManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto TextureManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void TextureManager::Reset()
    {
    }
}