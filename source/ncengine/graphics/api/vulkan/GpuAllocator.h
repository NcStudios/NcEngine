#pragma once

#include "NcVulkan.h"

#include "ncmath/Vector.h"

#include "utility/Memory.h"
#include <utility>

namespace nc::graphics::vulkan
{
    class Device;
    class GpuAllocator;
    class Instance;

    template<class T>
    class GpuAllocation
    {
        public:
            GpuAllocation() noexcept;
            GpuAllocation(T data, VmaAllocation allocation, GpuAllocator* allocator) noexcept;

            GpuAllocation(const GpuAllocation<T>&) = delete;
            GpuAllocation(GpuAllocation<T>&&) noexcept;
            GpuAllocation& operator=(const GpuAllocation<T>&) = delete;
            GpuAllocation<T>& operator=(GpuAllocation&&) noexcept;
            ~GpuAllocation() noexcept;

            operator T() const noexcept { return m_data; }
            auto Data() const noexcept -> T { return m_data; }
            auto Allocation() const noexcept -> VmaAllocation { return m_allocation; }
            auto GetInfo() const -> VmaAllocationInfo;
            void Release() noexcept;

        private:
            T m_data;
            VmaAllocation m_allocation;
            GpuAllocator* m_allocator;
    };

    class GpuAllocator
    {
        public:
            GpuAllocator(const Device* device, const Instance& instance);
            ~GpuAllocator() noexcept;
            GpuAllocator(const GpuAllocator&) = delete;
            GpuAllocator(GpuAllocator&&) = delete;
            GpuAllocator& operator=(const GpuAllocator&) = delete;
            GpuAllocator& operator=(GpuAllocator&&) = delete;

            auto PadBufferOffsetAlignment(uint32_t originalSize, vk::DescriptorType bufferType) -> uint32_t;

            auto Map(VmaAllocation allocation) const -> void*;
            void Unmap(VmaAllocation allocation) const;

            void CopyBuffer(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);
            auto CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, VmaMemoryUsage usageType) -> GpuAllocation<vk::Buffer>;
            auto CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::ImageCreateFlags imageFlags, uint32_t arrayLayers, uint32_t mipLevels, vk::SampleCountFlagBits numSamples) -> GpuAllocation<vk::Image>;
            auto CreateTexture(const unsigned char* pixels, uint32_t width, uint32_t height, uint32_t mipLevels, bool isNormal) -> GpuAllocation<vk::Image>;
            auto CreateTextureView(vk::Image image, uint32_t mipLevels, bool isNormal) -> vk::UniqueImageView;
            auto CreateCubeMapTexture(const unsigned char* pixels, uint32_t cubeMapSize, uint32_t sideLength) -> GpuAllocation<vk::Image>;
            auto CreateCubeMapTextureView(vk::Image image) -> vk::UniqueImageView;
            void Destroy(const GpuAllocation<vk::Buffer>& buffer) const;
            void Destroy(const GpuAllocation<vk::Image>& image) const;

        private:
            void GenerateMipMaps(vk::Image, uint32_t width, uint32_t height, uint32_t mipLevels);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);
            void TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, uint32_t layerCount, uint32_t mipLevels, vk::ImageLayout newLayout);
            const Device* m_device;
            VmaAllocator m_allocator;
            vk::PhysicalDeviceProperties m_deviceProperties;
    };

    template<class T>
    GpuAllocation<T>::GpuAllocation() noexcept
        : m_data{},
          m_allocation{},
          m_allocator{nullptr}
    {
    }

    template<class T>
    GpuAllocation<T>::GpuAllocation(T data, VmaAllocation allocation, GpuAllocator* allocator) noexcept
        : m_data{data},
          m_allocation{allocation},
          m_allocator{allocator}
    {
    }

    template<class T>
    GpuAllocation<T>::GpuAllocation(GpuAllocation<T>&& other) noexcept
        : m_data{std::exchange(other.m_data, T{})},
          m_allocation{std::exchange(other.m_allocation, VmaAllocation{})},
          m_allocator{std::exchange(other.m_allocator, nullptr)}
    {
    }

    template<class T>
    GpuAllocation<T>& GpuAllocation<T>::operator=(GpuAllocation<T>&& other) noexcept
    {
        if(m_allocator)
        {
            m_allocator->Destroy(*this);
        }

        m_data = std::exchange(other.m_data, T{});
        m_allocation = std::exchange(other.m_allocation, VmaAllocation{});
        m_allocator = std::exchange(other.m_allocator, nullptr);
        return *this;
    }

    template<class T>
    GpuAllocation<T>::~GpuAllocation() noexcept
    {
        if(m_allocator)
        {
            m_allocator->Destroy(*this);
        }
    }

    template<class T>
    void GpuAllocation<T>::Release() noexcept
    {
        if(m_allocator)
        {
            m_allocator->Destroy(*this);
        }

        m_data = T{};
        m_allocation = VmaAllocation{};
        m_allocator = nullptr;
    }
} // namespace nc::graphics::vulkan
