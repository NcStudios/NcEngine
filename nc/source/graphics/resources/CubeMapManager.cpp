#include "CubeMapManager.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    CubeMapManager::CubeMapManager(Graphics* graphics, uint32_t maxCubeMaps)
        : m_graphics{graphics},
          m_imageInfos{},
          m_descriptorSet{},
          m_descriptorSetLayout{},
          m_cubeMapSampler{},
          m_layout{},
          m_maxCubeMapsCount{maxCubeMaps},
          m_cubeMapsInitialized{false}
    {
        Initialize();
    }

    CubeMapManager::~CubeMapManager() noexcept
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        m_cubeMapSampler.reset();
    }

    void CubeMapManager::Initialize()
    {
        // Create and bind the descriptor set for the array of textures.
        std::array<vk::DescriptorSetLayoutBinding, 2u> layoutBindings
        {
          CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eSampler, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex),
          CreateDescriptorSetLayoutBinding(1, m_maxCubeMapsCount, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
        };

        std::array<vk::DescriptorBindingFlagsEXT, 2> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT(),
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound 
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_descriptorSet = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

        /** @todo Figure out why there are validation errors when creating sampler in the constructor. */
        m_cubeMapSampler = m_graphics->GetBasePtr()->CreateTextureSampler();
    }

    void CubeMapManager::Update(const std::vector<CubeMap>& data)
    {
        assert(data.size() < m_maxCubeMapsCount && !data.empty());

        std::array<vk::WriteDescriptorSet, 2> writes;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = m_cubeMapSampler.get();

        m_imageInfos = std::vector<vk::DescriptorImageInfo>(m_maxCubeMapsCount, CreateDescriptorImageInfo(&m_cubeMapSampler.get(), data.at(0).GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));

        writes[0].setDstBinding(0);
        writes[0].setDstArrayElement(0);
        writes[0].setDescriptorType(vk::DescriptorType::eSampler);
        writes[0].setDescriptorCount(1);
        writes[0].setDstSet(m_descriptorSet.get());
        writes[0].setPBufferInfo(0);
        writes[0].setPImageInfo(&samplerInfo);

        std::transform(data.cbegin(), data.cend(), m_imageInfos.begin(), [sampler = &m_cubeMapSampler.get()](const auto& cubeMap)
        {
            return CreateDescriptorImageInfo(sampler, cubeMap.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
        });

        writes[1].setDstBinding(1);
        writes[1].setDstArrayElement(0);
        writes[1].setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
        writes[1].setDescriptorCount(m_maxCubeMapsCount);
        writes[1].setDstSet(m_descriptorSet.get());
        writes[1].setPBufferInfo(0);
        writes[1].setPImageInfo(m_imageInfos.data());

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
    }

    auto CubeMapManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto CubeMapManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void CubeMapManager::Reset()
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        m_cubeMapSampler.reset();
        Initialize();
    }
}