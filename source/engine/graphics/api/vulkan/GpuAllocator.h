#pragma once

#ifdef __APPLE__
    #ifndef VK_USE_PLATFORM_METAL_EXT
    #define VK_USE_PLATFORM_METAL_EXT
    #endif 
#elif _WIN32
    #ifndef VK_USE_PLATFORM_WIN32_KHR
    #define VK_USE_PLATFORM_WIN32_KHR
    #endif
#elif __linux__
    #ifndef VK_USE_PLATFORM_XLIB_KHR
    #define VK_USE_PLATFORM_XLIB_KHR
    #endif
#else
    #error "Unknown platform"
#endif

#include "ncmath/Vector.h"

#include "vulkan/vk_mem_alloc.hpp"
#include "utility/Memory.h"

#include <utility>

namespace nc
{
namespace graphics
{
    class Device;
    class GpuAllocator;
    class Instance;

    /** @todo Add create image view (used in Attachment) */
    auto CreateTextureView(vk::Device device, vk::Image image, bool isNormal) -> vk::UniqueImageView;
    auto CreateCubeMapTextureView(vk::Device device, vk::Image image) -> vk::UniqueImageView;

    template<class T>
    class GpuAllocation
    {
        public:
            GpuAllocation() noexcept;
            GpuAllocation(T data, vma::Allocation allocation, GpuAllocator* allocator) noexcept;
            GpuAllocation(const GpuAllocation<T>&) = delete;
            GpuAllocation(GpuAllocation<T>&&) noexcept;
            GpuAllocation& operator=(const GpuAllocation<T>&) = delete;
            GpuAllocation<T>& operator=(GpuAllocation&&) noexcept;
            ~GpuAllocation() noexcept;

            operator T() const noexcept { return m_data; }
            auto Data() const noexcept -> T { return m_data; }
            auto Allocation() const noexcept -> vma::Allocation { return m_allocation; }
            auto GetInfo() const -> vma::AllocationInfo;
            void Release() noexcept;

        private:
            T m_data;
            vma::Allocation m_allocation;
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

            auto Map(vma::Allocation allocation) const -> void*;
            void Unmap(vma::Allocation allocation) const;

            void CopyBuffer(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);
            auto CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vma::MemoryUsage usageType) -> GpuAllocation<vk::Buffer>;
            auto CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::ImageCreateFlags imageFlags, uint32_t arrayLayers, vk::SampleCountFlagBits numSamples) -> GpuAllocation<vk::Image>;
            auto CreateTexture(const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal) -> GpuAllocation<vk::Image>;
            auto CreateCubeMapTexture(const unsigned char* pixels, uint32_t cubeMapSize, uint32_t sideLength) -> GpuAllocation<vk::Image>;

            void Destroy(const GpuAllocation<vk::Buffer>& buffer) const;
            void Destroy(const GpuAllocation<vk::Image>& image) const;

            template<class T>
            auto GetAllocationInfo(const GpuAllocation<T>& allocation) const -> vma::AllocationInfo
            {
                auto info = vma::AllocationInfo{};
                m_allocator.getAllocationInfo(allocation.Allocation(), &info);
                return info;
            }

        private:
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);
            void TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, uint32_t layerCount, vk::ImageLayout newLayout);
            const Device* m_device;
            vma::Allocator m_allocator;
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
    GpuAllocation<T>::GpuAllocation(T data, vma::Allocation allocation, GpuAllocator* allocator) noexcept
        : m_data{data},
          m_allocation{allocation},
          m_allocator{allocator}
    {
    }

    template<class T>
    GpuAllocation<T>::GpuAllocation(GpuAllocation<T>&& other) noexcept
        : m_data{std::exchange(other.m_data, T{})},
          m_allocation{std::exchange(other.m_allocation, vma::Allocation{})},
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
        m_allocation = std::exchange(other.m_allocation, vma::Allocation{});
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
        m_allocation = vma::Allocation{};
        m_allocator = nullptr;
    }

    template<class T>
    auto GpuAllocation<T>::GetInfo() const -> vma::AllocationInfo
    {
        return m_allocator->GetAllocationInfo(*this);
    }
} // namespace graphics
} // namespace nc
