#include "PointLightManager.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    PointLightManager::PointLightManager(Graphics* graphics, uint32_t maxPointLights)
        : m_pointLightsArrayBuffer{},
          m_descriptorSet{},
          m_descriptorSetLayout{},
          m_graphics{graphics},
          m_maxPointLights{maxPointLights}
    {
        Initialize();
    }

    PointLightManager::~PointLightManager() noexcept
    {
        m_pointLightsArrayBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void PointLightManager::Initialize()
    {
        auto base = m_graphics->GetBasePtr();

        std::array<vk::DescriptorSetLayoutBinding, 1u> layoutBindings
        {
            CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex )
        };
        
        std::array<vk::DescriptorBindingFlagsEXT, 1> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT()
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_pointLightsArrayBuffer = WriteableBuffer<nc::PointLightInfo>(m_graphics, (sizeof(nc::PointLightInfo) * m_maxPointLights));
        m_descriptorSet = CreateDescriptorSet(m_graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

		vk::DescriptorBufferInfo pointLightArrayInfo;
		pointLightArrayInfo.buffer = *m_pointLightsArrayBuffer.GetBuffer();
		pointLightArrayInfo.offset = 0;
		pointLightArrayInfo.range = sizeof(nc::PointLightInfo) * m_maxPointLights;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&pointLightArrayInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void PointLightManager::Update(const std::vector<PointLightInfo>& data)
    {
        m_pointLightsArrayBuffer.Map(data, [](PointLightInfo& info)
        {
            info.isInitialized = false;
        });
    }

    auto PointLightManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto PointLightManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void PointLightManager::Reset()
    {
        m_pointLightsArrayBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        Initialize();
    }
}