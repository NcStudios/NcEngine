#include "EnvironmentDataManager.h"
#include "graphics/vk/Initializers.h"
#include "graphics/resources/Environment.h"

namespace nc::graphics
{
    EnvironmentDataManager::EnvironmentDataManager(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors)
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

    void EnvironmentDataManager::Update(const std::vector<EnvironmentData>& data)
    {
       m_environmentDataBuffer->Bind(static_cast<const void*>(&data.at(0)), static_cast<uint32_t>(sizeof(EnvironmentData) * data.size()));
    }

    void EnvironmentDataManager::Reset()
    {
        Initialize();
    }
}