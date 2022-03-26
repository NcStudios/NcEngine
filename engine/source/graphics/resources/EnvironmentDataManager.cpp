#include "EnvironmentDataManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/Environment.h"

namespace nc::graphics
{
    EnvironmentDataManager::EnvironmentDataManager(uint32_t bindingSlot, GpuAllocator* allocator, shader_descriptor_sets* descriptors)
        : m_allocator{allocator},
          m_descriptors{descriptors},
          m_bindingSlot{bindingSlot}
    {
        Initialize();
    }

    EnvironmentDataManager::~EnvironmentDataManager() noexcept
    {
    }

    void EnvironmentDataManager::Initialize()
    {
        auto initialEnvironmentData = EnvironmentData{};
        initialEnvironmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
        initialEnvironmentData.skyboxTextureIndex = -1;
        auto dataVector = std::vector<EnvironmentData>{};
        dataVector.push_back(initialEnvironmentData);
        m_environmentDataBuffer = std::make_unique<UniformBuffer>(m_allocator, static_cast<const void*>(&dataVector.back()), static_cast<uint32_t>(sizeof(EnvironmentData) * dataVector.size()));

        m_descriptors->register_descriptor
        (
            m_bindingSlot,
            bind_frequency::per_frame,
            1,
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
            vk::DescriptorBindingFlagBitsEXT()
        );

        m_descriptors->update_buffer
        (
            bind_frequency::per_frame,
            m_environmentDataBuffer->GetBuffer(),
            sizeof(EnvironmentData),
            1,
            vk::DescriptorType::eUniformBuffer,
            m_bindingSlot
        );
    }

    void EnvironmentDataManager::Update(const std::vector<EnvironmentData>& data)
    {
       m_environmentDataBuffer->Bind(static_cast<const void*>(&data.at(0)), static_cast<uint32_t>(sizeof(EnvironmentData) * data.size()));
    }

    void EnvironmentDataManager::Reset()
    {
        Initialize();
    }
}