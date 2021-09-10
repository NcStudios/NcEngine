#pragma once

#include "graphics/Base.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    template<typename T>
    class WriteableBuffer
    {
        public:
            WriteableBuffer();
            WriteableBuffer(Graphics* graphics, uint32_t size);
            ~WriteableBuffer() noexcept;
            WriteableBuffer(WriteableBuffer&&);
            WriteableBuffer& operator = (WriteableBuffer&&);
            WriteableBuffer& operator = (const WriteableBuffer&) = delete;
            WriteableBuffer(const WriteableBuffer&) = delete;

            vk::Buffer* GetBuffer() noexcept;

            void Clear() noexcept;

            template<std::invocable<T&> Func>
            void Map(const std::vector<T>& dataToMap, Func&& myNuller);
            void Map(const std::vector<T>& dataToMap);

        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            uint32_t m_memorySize;
            vk::Buffer m_writeableBuffer;
    };

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer()
    : m_base{ nullptr },
      m_memoryIndex { 0 },
      m_memorySize { 0 },
      m_writeableBuffer { nullptr }
    {
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(Graphics* graphics, uint32_t size)
    : m_base{ graphics->GetBasePtr() },
      m_memoryIndex { 0 },
      m_memorySize{ 0 },
      m_writeableBuffer { nullptr }
    {
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu, &m_writeableBuffer);
    }

    template<typename T>
    WriteableBuffer<T>::~WriteableBuffer() noexcept
    {
        if (m_writeableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_writeableBuffer = nullptr;
        }

        m_base = nullptr;
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(WriteableBuffer&& other)
    : m_base{std::exchange(other.m_base, nullptr)},
      m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
      m_memorySize{std::exchange(other.m_memorySize, 0)},
      m_writeableBuffer{std::exchange(other.m_writeableBuffer, nullptr)}
    {
    }

    template<typename T>
    WriteableBuffer<T>& WriteableBuffer<T>::operator=(WriteableBuffer<T>&& other)
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_memorySize = std::exchange(other.m_memorySize, 0);
        m_writeableBuffer = std::exchange(other.m_writeableBuffer, nullptr);
        return *this;
    }

    template<typename T>
    vk::Buffer* WriteableBuffer<T>::GetBuffer() noexcept
    {
        return &m_writeableBuffer;
    }

    template<typename T>
    void WriteableBuffer<T>::Clear() noexcept
    {
        if (m_writeableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_writeableBuffer = nullptr;
        }

        m_base = nullptr;
    }

    template<typename T> 
    template<std::invocable<T&> Func>
    void WriteableBuffer<T>::Map(const std::vector<T>& dataToMap, Func&& myNuller)
    {
        auto allocation = *(m_base->GetBufferAllocation(m_memoryIndex));
        
        void* dataContainer;
        m_base->GetAllocator()->mapMemory(allocation, &dataContainer);

        T* mappedContainerHandle = (T*)dataContainer;

        auto dataToMapSize = dataToMap.size();

        for (uint32_t i = 0; i < static_cast<uint32_t>(dataToMapSize); ++i)
        {
            mappedContainerHandle[i] = dataToMap[i];
        }

        // Note: WriteableBuffer can have a dynamic array. This block handles the case where we are mapping less data than has been previously mapped.
        // If the dataToMapSize is empty, we must set the first item to "null" via myNuller which needs to be handled by the object being mapped.
        if (dataToMapSize == 0)
        {
            myNuller(mappedContainerHandle[0]);
        }

        // Note: WriteableBuffer can have a dynamic array. This block handles the case where we are mapping less data than has been previously mapped.
        // We must set the first "unmapped" item to "null" via myNuller which needs to be handled by the object being mapped.
        if (dataToMapSize < sizeof(m_writeableBuffer))
        {
            myNuller(mappedContainerHandle[dataToMapSize]);
        }

        m_base->GetAllocator()->unmapMemory(allocation);
    }

    template<typename T> 
    void WriteableBuffer<T>::Map(const std::vector<T>& dataToMap)
    {
        auto allocation = *(m_base->GetBufferAllocation(m_memoryIndex));
        void* dataContainer;
        m_base->GetAllocator()->mapMemory(allocation, &dataContainer);
        memcpy(dataContainer, dataToMap.data(), sizeof(T) * dataToMap.size());
        m_base->GetAllocator()->unmapMemory(allocation);
    }
}