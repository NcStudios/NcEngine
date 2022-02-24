#include "CubeMapManager.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    CubeMapManager::CubeMapManager(Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps)
        : m_graphics{graphics},
          m_descriptors{descriptors},
          m_imageInfos{},
          m_cubeMapSampler{},
          m_layout{},
          m_maxCubeMapsCount{maxCubeMaps},
          m_cubeMapsInitialized{false},
          m_bindingSlot{0}
    {
        Initialize();
    }

    CubeMapManager::~CubeMapManager() noexcept
    {
        m_cubeMapSampler.reset();
    }

    void CubeMapManager::Initialize()
    {
        m_bindingSlot = m_descriptors->RegisterDescriptor
        (
            BindFrequency::PerFrame,
            m_imageInfos,
            m_maxCubeMapsCount,
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
        );
    }

    void CubeMapManager::Update(const std::vector<CubeMap>& data)
    {
        assert(data.size() < m_maxCubeMapsCount && !data.empty());

        m_imageInfos = std::vector<vk::DescriptorImageInfo>(m_maxCubeMapsCount, CreateDescriptorImageInfo(&m_cubeMapSampler.get(), data.at(0).GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));

        std::transform(data.cbegin(), data.cend(), m_imageInfos.begin(), [sampler = &m_cubeMapSampler.get()](const auto& cubeMap)
        {
            return CreateDescriptorImageInfo(sampler, cubeMap.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
        });

        m_descriptors->UpdateDescriptor
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