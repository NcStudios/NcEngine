#include "ObjectDataShaderResource.h"
#include "graphics/vk/Initializers.h"

namespace nc::graphics
{
    ObjectDataShaderResource::ObjectDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxRenderers)
        : m_allocator { allocator },
          m_descriptors{ descriptors },
          m_objectsDataBuffer{},
          m_maxObjects{ maxRenderers },
          m_bindingSlot{ bindingSlot }
    {
        Initialize();
    }

    ObjectDataShaderResource::~ObjectDataShaderResource() noexcept
    {
        m_objectsDataBuffer.Clear();
    }

    void ObjectDataShaderResource::Initialize()
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

    void ObjectDataShaderResource::Update(const std::vector<ObjectData>& data)
    {
        m_objectsDataBuffer.Map(data);
    }

    void ObjectDataShaderResource::Reset()
    {
        m_objectsDataBuffer.Clear();
        Initialize();
    }
}