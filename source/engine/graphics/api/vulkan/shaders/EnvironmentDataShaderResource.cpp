#include "EnvironmentDataShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/shader_resource/EnvironmentData.h"

namespace nc::graphics
{
    EnvironmentDataShaderResource::EnvironmentDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors)
        : m_allocator{allocator},
          m_descriptors{descriptors},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    EnvironmentDataShaderResource::~EnvironmentDataShaderResource() noexcept
    {
    }

    void EnvironmentDataShaderResource::Initialize()
    {
        auto initialEnvironmentData = EnvironmentData{};
        initialEnvironmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
        initialEnvironmentData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
        auto dataVector = std::vector<EnvironmentData>{};
        dataVector.push_back(initialEnvironmentData);
        m_environmentDataBuffer = std::make_unique<UniformBuffer>(m_allocator, static_cast<const void*>(&dataVector.back()), static_cast<uint32_t>(sizeof(EnvironmentData) * dataVector.size()));

        m_descriptors->RegisterDescriptor
        (
            m_bindingSlot,
            BindFrequency::per_frame,
            1,
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
            vk::DescriptorBindingFlagBitsEXT()
        );

        m_descriptors->UpdateBuffer
        (
            BindFrequency::per_frame,
            m_environmentDataBuffer->GetBuffer(),
            sizeof(EnvironmentData),
            1,
            vk::DescriptorType::eUniformBuffer,
            m_bindingSlot
        );
    }

    void EnvironmentDataShaderResource::Update(const std::vector<EnvironmentData>& data)
    {
       m_environmentDataBuffer->Bind(static_cast<const void*>(&data.at(0)), static_cast<uint32_t>(sizeof(EnvironmentData) * data.size()));
    }

    void EnvironmentDataShaderResource::Reset()
    {
        Initialize();
    }
}
