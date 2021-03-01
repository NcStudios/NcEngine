#pragma once

#include "math/Vector2.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class Instance;

    // How many frames can be rendered concurrently.
    // Each frame requires its own pair of semaphores.
    const uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    enum class QueueFamilyType : uint8_t
    {
        GraphicsFamily,
        PresentFamily
    };

    enum class SemaphoreType : uint8_t
    {
        RenderReady,
        PresentReady
    };

    enum class FenceType : uint8_t
    {
        FramesInFlight,
        ImagesInFlight
    };

    namespace vertex { struct Vertex; }

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

    class Device
    {
        public:
            Device(const vulkan::Instance& instance, Vector2 dimensions);
            ~Device();

            const vk::Device& GetDevice() const noexcept;
            const Vector2 GetSwapChainExtentDimensions() const noexcept;
            const vk::Extent2D& GetSwapChainExtent() const noexcept;
            const vk::Format& GetSwapChainImageFormat() const noexcept;
            const std::vector<vk::ImageView>& GetSwapChainImageViews() const noexcept;
            uint32_t GetNextRenderReadyImageIndex(bool& isSwapChainValid);
            const std::vector<vk::Semaphore>& GetSemaphores(SemaphoreType semaphoreType) const noexcept;
            const vk::CommandPool& GetCommandPool() const noexcept;
            const vk::Queue& GetQueue(QueueFamilyType type) const noexcept;
            uint32_t GetFrameIndex() const noexcept;
            const std::vector<vk::Fence>& GetFences(FenceType fenceType) const noexcept;
            const vma::Allocation& GetAllocation(uint32_t bufferId) const;

            void Present(uint32_t imageIndex, bool& isSwapChainValid);
            void IncrementFrameIndex();
            void WaitForFrameFence();
            void WaitForImageFence(uint32_t imageIndex);
            void SyncImageAndFrameFence(uint32_t imageIndex);
            void ResetFrameFence();
            void CreateSwapChain(Vector2 dimensions);
            void CleanupSwapChain();
            void DestroyBuffer(uint32_t id);
            void MapMemory(uint32_t bufferId, std::vector<vertex::Vertex> vertices, size_t size);
            void MapMemory(uint32_t bufferId, std::vector<uint32_t> indices, size_t size);
            uint32_t CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, bool isStaging, vk::Buffer* createdBuffer);

        private:
            
            void CreatePhysicalDevice();
            void CreateLogicalDevice();
            void CreateCommandPool();
            void CreateSynchronizationObjects();
            void CreateAllocator();

            const vk::Instance& m_instance;
            const vk::SurfaceKHR& m_surface;
            vk::Device m_device;
            vk::PhysicalDevice m_physicalDevice;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;
            vk::SwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapChainImages;
            vk::Format m_swapChainImageFormat;
            vk::Extent2D m_swapChainExtent;
            std::vector<vk::ImageView> m_swapChainImageViews;
            vk::CommandPool m_commandPool;
            std::vector<vk::CommandBuffer> m_commandBuffers;
            std::vector<vk::Semaphore> m_imageRenderReadySemaphores; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Controls when the swapchain image can be written to.
            std::vector<vk::Semaphore> m_imagePresentReadySemaphores; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Controls when the swapchain image can be presented back to the swapchain.
            std::vector<vk::Fence> m_framesInFlightFences; // One per concurrent frame. (MAX_FRAMES_IN_FLIGHT). Synchronizes the submission of the queues from the CPU with the completion of the queues on the GPU.
            std::vector<vk::Fence> m_imagesInFlightFences; 
            uint32_t m_currentFrameIndex; // Used to select which pair of semaphores and which fence to use as each frame in MAX_FRAMES_IN_FLIGHT requires its own pair of semaphores and fence.
            vma::Allocator m_allocator;
            std::unordered_map<uint32_t, std::pair<vk::Buffer, vma::Allocation>> m_buffers;
            uint32_t m_bufferIndex;
    };
}