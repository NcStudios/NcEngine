#include "EnvironmentDataManager.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/resources/EnvironmentImpl.h"
#include "debug/Utils.h"

#include <array>
#include <iostream>

namespace nc::graphics
{
    EnvironmentDataManager::EnvironmentDataManager(Graphics* graphics)
        : m_graphics{graphics},
          m_descriptorSet{},
          m_descriptorSetLayout{}
    {
        Initialize();
    }

    EnvironmentDataManager::~EnvironmentDataManager() noexcept
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void EnvironmentDataManager::Initialize()
    {
        auto base = m_graphics->GetBasePtr();

        std::array<vk::DescriptorSetLayoutBinding, 1u> layoutBindings
        {
            CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
        };
        
        std::array<vk::DescriptorBindingFlagsEXT, 1> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT()
        };

        auto initialEnvironmentData = EnvironmentData{};
        initialEnvironmentData.cameraWorldPosition = Vector3{-0.0f, 4.0f, -6.4f};
        initialEnvironmentData.skyboxTextureIndex = -1;
        auto dataVector = std::vector<EnvironmentData>{};

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_descriptorSet = CreateDescriptorSet(m_graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());
        dataVector.push_back(initialEnvironmentData);
        m_environmentDataBuffer = std::make_unique<UniformBuffer>(m_graphics, dataVector.back());

        vk::DescriptorBufferInfo environmentBufferInfo;
        environmentBufferInfo.buffer = *m_environmentDataBuffer->GetBuffer();
        environmentBufferInfo.offset = 0;
        environmentBufferInfo.range = sizeof(EnvironmentData);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&environmentBufferInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void EnvironmentDataManager::Update(const std::vector<EnvironmentData>& data)
    {
       m_environmentDataBuffer->Bind(m_graphics, data.at(0));
    }

    auto EnvironmentDataManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto EnvironmentDataManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void EnvironmentDataManager::Reset()
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        Initialize();
    }
}