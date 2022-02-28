#pragma once

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Vertex.h"
#include "graphics/Environment.h"
#include "graphics/resources/EnvironmentImpl.h"

#include <vector>
#include "vulkan/vk_mem_alloc.hpp"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    class Graphics;

    // Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
    class UniformBuffer
    {
        public:
            UniformBuffer();
            template <typename T>
            UniformBuffer(nc::graphics::Graphics* graphics, const T& data);
            ~UniformBuffer() noexcept;
            UniformBuffer(UniformBuffer&&);
            UniformBuffer& operator=(UniformBuffer&&);
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(const UniformBuffer&) = delete;
            
            vk::Buffer* GetBuffer();
            template <typename T>
            void Bind(nc::graphics::Graphics* graphics, const T& data);
            void Clear();

        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            vk::Buffer m_uniformBuffer;
    };

    template <typename T>
    void UniformBuffer::Bind(Graphics* graphics, const T& data)
    {
        m_base = graphics->GetBasePtr();

        auto size = m_base->PadBufferOffsetAlignment(static_cast<uint32_t>(sizeof(T)), vk::DescriptorType::eUniformBuffer);

        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(m_memoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, &data, size);
        allocator->unmapMemory(*allocation);
    }

    template <typename T>
    UniformBuffer::UniformBuffer(Graphics* graphics, const T& data)
        : m_memoryIndex{ 0 },
        m_uniformBuffer{ nullptr }
    {
        m_base = graphics->GetBasePtr();
        auto size = m_base->PadBufferOffsetAlignment(static_cast<uint32_t>(sizeof(T)), vk::DescriptorType::eUniformBuffer);
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu, &m_uniformBuffer);

        Bind(graphics, data);
    }
}