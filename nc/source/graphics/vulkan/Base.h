#pragma once

#include "math/Vector2.h"
#include "platform/win32/NcWin32.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.hpp"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class Instance;
    namespace vertex { struct Vertex; }

    // How many frames can be rendered concurrently.
    // Each frame requires its own pair of semaphores.
    const uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    enum class QueueFamilyType : uint8_t
    {
        GraphicsFamily,
        PresentFamily
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class QueueFamilyIndices
    {
        public:
            QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
            bool IsComplete() const;
            bool IsSeparatePresentQueue() const;
            uint32_t GetQueueFamilyIndex(QueueFamilyType type) const;

        private:
            std::optional<uint32_t> m_graphicsFamily;
            std::optional<uint32_t> m_presentFamily;
            bool m_isSeparatePresentQueue;
    };

    class Base
    {
        public:
            Base(HWND hwnd, HINSTANCE hinstance);
            ~Base();

            const vk::Device& GetDevice() const noexcept;
            const vk::PhysicalDevice& GetPhysicalDevice() const noexcept;
            const vk::SurfaceKHR& GetSurface() const noexcept;
            const vk::CommandPool& GetCommandPool() const noexcept;
            void FreeCommandBuffers();
            const vk::Queue& GetQueue(QueueFamilyType type) const noexcept;

            uint32_t CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, bool isStaging, vk::Buffer* createdBuffer);
            void DestroyBuffer(uint32_t id);
            void MapMemory(uint32_t bufferId, std::vector<vertex::Vertex> vertices, size_t size);
            void MapMemory(uint32_t bufferId, std::vector<uint32_t> indices, size_t size);
            const SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;

        private:
            
            void CreateInstance();
            void CreateSurface(HWND hwnd, HINSTANCE hinstance);
            void CreateAllocator();
            void CreateCommandPool();
            void CreateLogicalDevice();
            void CreatePhysicalDevice();
            void CreateCommandQueues();

            vk::Instance m_instance;
            vk::SurfaceKHR m_surface;
            vk::Device m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;
            vk::CommandPool m_commandPool;
            std::vector<vk::CommandBuffer> m_commandBuffers;
            vma::Allocator m_allocator;
            std::unordered_map<uint32_t, std::pair<vk::Buffer, vma::Allocation>> m_buffers;
            uint32_t m_bufferIndex;
    };
}