#pragma once

#include "graphics/vulkan/Base.h"
#include "graphics/Graphics2.h"

namespace nc::graphics::vulkan
{
    template<typename T>
    class WriteableBuffer
    {
        public:
            WriteableBuffer();
            WriteableBuffer(Graphics2* graphics, uint32_t size);
            WriteableBuffer(WriteableBuffer&&);
            WriteableBuffer& operator = (WriteableBuffer&&);
            WriteableBuffer& operator = (const WriteableBuffer&) = delete;
            WriteableBuffer(const WriteableBuffer&) = delete;
            ~WriteableBuffer();

            vk::Buffer* GetBuffer();

            void Clear();
            void Map(const std::vector<T>& data);

        private:
            vulkan::Base* m_base;
            uint32_t m_memoryIndex;
            vk::Buffer m_writeableBuffer;
    };

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer()
    : m_memoryIndex { 0 },
      m_writeableBuffer { nullptr }
    {
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(Graphics2* graphics, uint32_t size)
    : m_base{ graphics->GetBasePtr() }
    {
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu, &m_writeableBuffer);
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(WriteableBuffer&& other)
    : m_base{std::exchange(other.m_base, nullptr)},
      m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
      m_writeableBuffer{std::exchange(other.m_writeableBuffer, nullptr)}
    {
    }

    template<typename T>
    WriteableBuffer<T>& WriteableBuffer<T>::operator = (WriteableBuffer<T>&& other)
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_writeableBuffer = std::exchange(other.m_writeableBuffer, nullptr);
        return *this;
    }

    template<typename T>
    WriteableBuffer<T>::~WriteableBuffer()
    {
        Clear();
    }

    template<typename T>
    vk::Buffer* WriteableBuffer<T>::GetBuffer()
    {
        return &m_writeableBuffer;
    }

    template<typename T>
    void WriteableBuffer<T>::Clear()
    {
        if (m_writeableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_writeableBuffer = nullptr;
        }

        m_base = nullptr;
    }

    template<typename T>
    void WriteableBuffer<T>::Map(const std::vector<T>& dataToMap)
    {
        auto allocation = *(m_base->GetBufferAllocation(m_memoryIndex));
        
        void* dataContainer;
        m_base->GetAllocator()->mapMemory(allocation, &dataContainer);

        T* mappedContainerHandle = (T*)dataContainer;

        for (uint32_t i = 0; i < static_cast<uint32_t>(dataToMap.size()); ++i)
        {
            mappedContainerHandle[i] = dataToMap[i];
        }

        m_base->GetAllocator()->unmapMemory(allocation);
    }
}