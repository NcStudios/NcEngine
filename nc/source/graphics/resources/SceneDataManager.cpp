#include "SceneDataManager.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "debug/Utils.h"

#include <array>

namespace nc::graphics
{
    SceneDataManager::SceneDataManager(Graphics* graphics)
        : m_graphics{graphics},
          m_descriptorSet{},
          m_descriptorSetLayout{}
    {
        Initialize();
    }

    SceneDataManager::~SceneDataManager() noexcept
    {
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void SceneDataManager::Initialize()
    {

    }

    void SceneDataManager::Update(const std::vector<SceneData>& data)
    {
        auto base = m_graphics->GetBasePtr();

        std::array<vk::DescriptorSetLayoutBinding, 1u> layoutBindings
        {
            CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
        };
        
        std::array<vk::DescriptorBindingFlagsEXT, 1> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT()
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_descriptorSet = CreateDescriptorSet(m_graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());
        m_sceneDataBuffer = std::make_unique<ImmutableBuffer>(m_graphics, data.at(0));

		vk::DescriptorBufferInfo sceneBufferInfo;
		sceneBufferInfo.buffer = *m_sceneDataBuffer->GetBuffer();
		sceneBufferInfo.offset = 0;
		sceneBufferInfo.range = sizeof(SceneData);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&sceneBufferInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    auto SceneDataManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto SceneDataManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void SceneDataManager::Reset()
    {
    }
}