#include "ObjectDataManager.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ObjectDataManager::ObjectDataManager(Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxRenderers)
        : m_graphics { graphics },
          m_descriptors{ descriptors },
          m_objectsDataBuffer{},
          m_maxObjects{ maxRenderers }
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
        m_objectsDataBuffer = WriteableBuffer<ObjectData>(m_graphics, objectsSize);

        m_descriptors->RegisterDescriptor
        (
            BindFrequency::PerFrame,
            m_objectsDataBuffer.GetBuffer(),
            objectsSize,
            1,
            vk::DescriptorType::eStorageBuffer,
            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex
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