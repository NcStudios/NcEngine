#include "EnvironmentDataManager.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/resources/EnvironmentImpl.h"
#include "debug/Utils.h"

#include <array>
#include <iostream>

namespace nc::graphics
{
    EnvironmentDataManager::EnvironmentDataManager(Graphics* graphics, ShaderDescriptorSets* descriptors)
        : m_graphics{graphics},
          m_descriptors{ descriptors }
    {
        Initialize();
    }

    EnvironmentDataManager::~EnvironmentDataManager() noexcept
    {
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
        dataVector.push_back(initialEnvironmentData);
        m_environmentDataBuffer = std::make_unique<UniformBuffer>(m_graphics, dataVector.back());

        m_descriptors->RegisterDescriptor
        (
            BindFrequency::PerFrame,
            m_environmentDataBuffer->GetBuffer(),
            1,
            1,
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex
        );
    }

    void EnvironmentDataManager::Update(const std::vector<EnvironmentData>& data)
    {
       m_environmentDataBuffer->Bind(m_graphics, data.at(0));
    }

    void EnvironmentDataManager::Reset()
    {
        Initialize();
    }
}