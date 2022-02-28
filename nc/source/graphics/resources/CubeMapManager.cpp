#include "CubeMapManager.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    CubeMapManager::CubeMapManager(uint32_t bindingSlot, Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps)
        : m_graphics{graphics},
          m_descriptors{descriptors},
          m_imageInfos{},
          m_cubeMapSampler{},
          m_layout{},
          m_maxCubeMapsCount{maxCubeMaps},
          m_cubeMapsInitialized{false},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    CubeMapManager::~CubeMapManager() noexcept
    {
        m_cubeMapSampler.reset();
    }

    void CubeMapManager::Initialize()
    {
        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::PerFrame,
            m_maxCubeMapsCount,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
        );
    }

    void CubeMapManager::Update(const std::vector<CubeMap>& data)
    {
        assert(data.size() < m_maxCubeMapsCount && !data.empty());

        m_cubeMapSampler = m_graphics->GetBasePtr()->CreateTextureSampler();

        m_imageInfos = std::vector<vk::DescriptorImageInfo>(m_maxCubeMapsCount, CreateDescriptorImageInfo(&m_cubeMapSampler.get(), data.at(0).GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));

        std::transform(data.cbegin(), data.cend(), m_imageInfos.begin(), [sampler = &m_cubeMapSampler.get()](const auto& cubeMap)
        {
            return CreateDescriptorImageInfo(sampler, cubeMap.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
        });

        m_descriptors->UpdateImage
        (
            BindFrequency::PerFrame,
            m_imageInfos,
            m_maxCubeMapsCount,
            vk::DescriptorType::eCombinedImageSampler,
            m_bindingSlot
        );
    }

    void CubeMapManager::Reset()
    {
        m_cubeMapSampler.reset();
        Initialize();
    }
}