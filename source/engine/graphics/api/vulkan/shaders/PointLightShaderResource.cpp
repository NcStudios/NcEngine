#include "PointLightShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics
{
    PointLightShaderResource::PointLightShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxPointLights)
        : m_pointLightsArrayBuffer{nullptr},
          m_descriptors{descriptors},
          m_allocator{allocator},
          m_maxPointLights{maxPointLights},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    PointLightShaderResource::~PointLightShaderResource() noexcept
    {
        m_pointLightsArrayBuffer->Clear();
    }

    void PointLightShaderResource::Initialize()
    {
        const auto bufferSize = static_cast<uint32_t>(sizeof(PointLightData) * m_maxPointLights);

        if (m_pointLightsArrayBuffer == nullptr)
        {
            m_pointLightsArrayBuffer = std::make_unique<WriteableBuffer<PointLightData>>(m_allocator, bufferSize);
        }

        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::per_frame,
            1,
            vk::DescriptorType::eStorageBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
            vk::DescriptorBindingFlagBitsEXT()
        );

        m_descriptors->UpdateBuffer
        (
            BindFrequency::per_frame,
            m_pointLightsArrayBuffer->GetBuffer(),
            bufferSize,
            1,
            vk::DescriptorType::eStorageBuffer,
            m_bindingSlot
        );
    }

    void PointLightShaderResource::Update(const std::vector<PointLightData>& data)
    {
        m_pointLightsArrayBuffer->Map(data, [](PointLightData& info)
        {
            info.isInitialized = false;
        });
    }

    void PointLightShaderResource::Reset()
    {
        // // m_pointLightsArrayBuffer.Clear();
        Initialize();
    }
}