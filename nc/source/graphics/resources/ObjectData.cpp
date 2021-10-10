#include "ObjectData.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ObjectDataManager::~ObjectDataManager() noexcept
    {
        m_objectsDataBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void ObjectDataManager::Initialize(Graphics* graphics)
    {
        /** @todo MAX_OBJECTS shouldn't be hardcoded right? */

        const uint32_t MAX_OBJECTS = 100000;
        const uint32_t objectsSize = (sizeof(ObjectData) * MAX_OBJECTS);
        auto base = graphics->GetBasePtr();

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)};
        m_descriptorSetLayout = CreateDescriptorSetLayout(graphics, layoutBindings, vk::DescriptorBindingFlagsEXT());
        m_objectsDataBuffer = WriteableBuffer<ObjectData>(graphics, objectsSize); //@todo: not hard code upper bound
        m_descriptorSet = CreateDescriptorSet(graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

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

    void ObjectDataManager::Reset(Graphics* graphics)
    {
        m_objectsDataBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        Initialize(graphics);
    }
}