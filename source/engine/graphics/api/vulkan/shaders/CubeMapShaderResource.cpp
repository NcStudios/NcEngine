#include "CubeMapShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
CubeMapShaderResource::CubeMapShaderResource(uint32_t bindingSlot, vk::Device device, ShaderDescriptorSets* descriptors, uint32_t maxCubeMaps)
    : m_device{device},
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

void CubeMapShaderResource::Initialize()
{
    m_descriptors->RegisterDescriptor
    (
        m_bindingSlot,
        BindFrequency::per_frame,
        m_maxCubeMapsCount,
        vk::DescriptorType::eCombinedImageSampler,
        vk::ShaderStageFlagBits::eFragment,
        vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
    );
}

void CubeMapShaderResource::Update(const std::vector<CubeMap>& data)
{
    assert(data.size() < m_maxCubeMapsCount && !data.empty());

    m_cubeMapSampler = graphics::CreateTextureSampler(m_device, vk::SamplerAddressMode::eRepeat);

    m_imageInfos = std::vector<vk::DescriptorImageInfo>();

    std::transform(data.cbegin(), data.cend(), std::back_inserter(m_imageInfos), [sampler = m_cubeMapSampler.get()](const auto& cubeMap)
    {
        return CreateDescriptorImageInfo(sampler, cubeMap.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
    });

    m_descriptors->UpdateImage
    (
        BindFrequency::per_frame,
        m_imageInfos,
        static_cast<uint32_t>(data.size()),
        vk::DescriptorType::eCombinedImageSampler,
        m_bindingSlot
    );
}

void CubeMapShaderResource::Reset()
{
    m_cubeMapSampler.reset();
    Initialize();
}
} // namespace nc::graphics