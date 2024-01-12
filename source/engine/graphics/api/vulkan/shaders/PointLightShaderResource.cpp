#include "PointLightShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/shader_resource/PointLightData.h"

#include <algorithm>

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
        auto fullVector =std::vector<PointLightData>{};
        fullVector.reserve(m_maxPointLights);
        fullVector.insert(fullVector.begin(), data.begin(), data.end());

        auto uninitializedPointLight = PointLightData{};
        uninitializedPointLight.isInitialized = false;

        // Fill the rest of the buffer with default values to satisfy VK_CmdDrawIndexed_None_02669
        std::generate_n(std::back_inserter(fullVector), m_maxPointLights - static_cast<uint32_t>(fullVector.size()), [&uninitializedPointLight]() -> PointLightData& { return uninitializedPointLight; });

        m_pointLightsArrayBuffer->Map(fullVector, [](PointLightData& info)
        {
            // After mapping, set all lights to uninitialized.
            info.isInitialized = false;
        });
    }

    void PointLightShaderResource::Reset()
    {
        // // m_pointLightsArrayBuffer.Clear();
        Initialize();
    }
}