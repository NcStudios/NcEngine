#include "PointLightManager.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    PointLightManager::PointLightManager(uint32_t bindingSlot, Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxPointLights)
        : m_pointLightsArrayBuffer{nullptr},
          m_descriptors{descriptors},
          m_graphics{graphics},
          m_maxPointLights{maxPointLights},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    PointLightManager::~PointLightManager() noexcept
    {
        m_pointLightsArrayBuffer->Clear();
    }

    void PointLightManager::Initialize()
    {
        auto base = m_graphics->GetBasePtr();
        const uint32_t pointLightsSize = (sizeof(nc::PointLightInfo) * m_maxPointLights);

        if (m_pointLightsArrayBuffer == nullptr)
        {
            m_pointLightsArrayBuffer = std::make_unique<WriteableBuffer<nc::PointLightInfo>>(m_graphics, pointLightsSize);
        }

        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::PerFrame,
            1,
            vk::DescriptorType::eStorageBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
            vk::DescriptorBindingFlagBitsEXT()
        );

        m_descriptors->UpdateBuffer
        (
            BindFrequency::PerFrame,
            m_pointLightsArrayBuffer->GetBuffer(),
            pointLightsSize,
            1,
            vk::DescriptorType::eStorageBuffer,
            m_bindingSlot
        );
    }

    void PointLightManager::Update(const std::vector<PointLightInfo>& data)
    {
        m_pointLightsArrayBuffer->Map(data, [](PointLightInfo& info)
        {
            info.isInitialized = false;
        });
    }

    void PointLightManager::Reset()
    {
        // // m_pointLightsArrayBuffer.Clear();
        Initialize();
    }
}