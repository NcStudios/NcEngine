#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics
{
    template<typename T>
    class WriteableBuffer
    {
        public:
            WriteableBuffer();
            WriteableBuffer(GpuAllocator* allocator, uint32_t size);
            WriteableBuffer(WriteableBuffer&&) noexcept;
            WriteableBuffer& operator=(WriteableBuffer&&) noexcept;
            WriteableBuffer& operator=(const WriteableBuffer&) = delete;
            WriteableBuffer(const WriteableBuffer&) = delete;

            vk::Buffer GetBuffer() const noexcept;

            void Clear() noexcept;

            template<std::invocable<T&> Func>
            void Map(const std::vector<T>& dataToMap, Func&& myNuller);
            void Map(const std::vector<T>& dataToMap);

        private:
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Buffer> m_buffer;
            uint32_t m_memorySize;
    };

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer()
        : m_allocator{nullptr},
          m_buffer{},
          m_memorySize{0}
    {
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(GpuAllocator* allocator, uint32_t size)
        : m_allocator{allocator},
          m_buffer{},
          m_memorySize{0}
    {
        size = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eStorageBuffer);
        m_buffer = m_allocator->CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu);
    }

    template<typename T>
    WriteableBuffer<T>::WriteableBuffer(WriteableBuffer&& other) noexcept
        : m_allocator{std::exchange(other.m_allocator, nullptr)},
          m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})},
          m_memorySize{std::exchange(other.m_memorySize, 0)}
    {
    }

    template<typename T>
    WriteableBuffer<T>& WriteableBuffer<T>::operator=(WriteableBuffer<T>&& other) noexcept
    {
        m_allocator = std::exchange(other.m_allocator, nullptr);
        m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
        m_memorySize = std::exchange(other.m_memorySize, 0);
        return *this;
    }

    template<typename T>
    vk::Buffer WriteableBuffer<T>::GetBuffer() const noexcept
    {
        return m_buffer;
    }

    template<typename T>
    void WriteableBuffer<T>::Clear() noexcept
    {
        m_buffer.Release();
        m_memorySize = 0;
        m_allocator = nullptr;
    }

    template<typename T> 
    template<std::invocable<T&> Func>
    void WriteableBuffer<T>::Map(const std::vector<T>& dataToMap, Func&& myNuller)
    {
        auto allocation = m_buffer.Allocation();
        void* dataContainer = m_allocator->Map(allocation);
        T* mappedContainerHandle = (T*)dataContainer;
        auto dataToMapSize = static_cast<uint32_t>(dataToMap.size());

        for (uint32_t i = 0; i < dataToMapSize; ++i)
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
        if (dataToMapSize < m_memorySize)
        {
            myNuller(mappedContainerHandle[dataToMapSize]);
        }

        m_memorySize = dataToMapSize;
        m_allocator->Unmap(allocation);
    }

    template<typename T> 
    void WriteableBuffer<T>::Map(const std::vector<T>& dataToMap)
    {
        auto allocation = m_buffer.Allocation();
        void* dataContainer = m_allocator->Map(allocation);
        memcpy(dataContainer, dataToMap.data(), sizeof(T) * dataToMap.size());
        m_allocator->Unmap(allocation);
    }
}