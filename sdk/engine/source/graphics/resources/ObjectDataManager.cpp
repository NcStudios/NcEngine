#include "ObjectDataManager.h"
#include "graphics/vk/Initializers.h"

namespace nc::graphics
{
    ObjectDataManager::ObjectDataManager(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxRenderers)
        : m_allocator { allocator },
          m_descriptors{ descriptors },
          m_objectsDataBuffer{},
          m_maxObjects{ maxRenderers },
          m_bindingSlot{ bindingSlot }
    {
        Initialize();
    }

    ObjectDataManager::~ObjectDataManager() noexcept
    {
        m_objectsDataBuffer.Clear();
    }

    void ObjectDataManager::Initialize()
    {
        const uint32_t objectsSize = (sizeof(ObjectData) * m_maxObjects);
        m_objectsDataBuffer = WriteableBuffer<ObjectData>(m_allocator, objectsSize);

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
            m_objectsDataBuffer.GetBuffer(),
            objectsSize,
            1,
            vk::DescriptorType::eStorageBuffer,
            m_bindingSlot
        );
    }

    void ObjectDataManager::Update(const std::vector<ObjectData>& data)
    {
        m_objectsDataBuffer.Map(data);
    }

    void ObjectDataManager::Reset()
    {
        m_objectsDataBuffer.Clear();
        Initialize();
    }
}