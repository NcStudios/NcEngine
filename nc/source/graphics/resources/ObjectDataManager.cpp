#include "ObjectDataManager.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ObjectDataManager::ObjectDataManager(Graphics* graphics, uint32_t maxRenderers)
        : m_objectsDataBuffer{},
          m_descriptorSet{},
          m_descriptorSetLayout{},
          m_graphics{graphics},
          m_maxObjects{maxRenderers}
    {
        Initialize();
    }

    ObjectDataManager::~ObjectDataManager() noexcept
    {
        m_objectsDataBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void ObjectDataManager::Initialize()
    {
        const uint32_t objectsSize = (sizeof(ObjectData) * m_maxObjects);
        auto base = m_graphics->GetBasePtr();

        std::array<vk::DescriptorSetLayoutBinding, 1u> layoutBindings
        {
            CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
        };

        std::array<vk::DescriptorBindingFlagsEXT, 1> layoutBindingFlags
        {  
            vk::DescriptorBindingFlagsEXT()
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(m_graphics, layoutBindings, layoutBindingFlags);
        m_objectsDataBuffer = WriteableBuffer<ObjectData>(m_graphics, objectsSize); //@todo: not hard code upper bound
        m_descriptorSet = CreateDescriptorSet(m_graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

		vk::DescriptorBufferInfo objectsDataBufferInfo;
		objectsDataBufferInfo.buffer = *m_objectsDataBuffer.GetBuffer();
		objectsDataBufferInfo.offset = 0;
		objectsDataBufferInfo.range = objectsSize;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&objectsDataBufferInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void ObjectDataManager::Update(const std::vector<ObjectData>& data)
    {
        m_objectsDataBuffer.Map(data);
    }

    auto ObjectDataManager::GetDescriptorSet() -> vk::DescriptorSet*
    {
        return &m_descriptorSet.get();
    }

    auto ObjectDataManager::GetDescriptorSetLayout() -> vk::DescriptorSetLayout*
    {
        return &m_descriptorSetLayout.get();
    }

    void ObjectDataManager::Reset()
    {
        m_objectsDataBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        Initialize();
    }
}