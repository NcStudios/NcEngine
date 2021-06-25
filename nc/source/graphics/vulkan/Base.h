#pragma once

#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vk_mem_alloc.hpp"
#include "imgui/imgui_impl_vulkan.h"

typedef unsigned char stbi_uc;

namespace nc::graphics::vulkan
{
    class Instance;
    class Commands;
    class TexturesData;
    struct Vertex;

    // How many frames can be rendered concurrently.
    // Each frame requires its own pair of semaphores.
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

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
            const vk::Queue& GetQueue(QueueFamilyType type) const noexcept;
            const vk::Format& GetDepthFormat() const noexcept;
            vk::DescriptorPool* GetRenderingDescriptorPoolPtr() noexcept;
            vma::Allocator* GetAllocator() noexcept;
            vma::Allocation* GetBufferAllocation(uint32_t index);

            uint32_t CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vma::MemoryUsage memoryUsageType, vk::Buffer* createdBuffer);
            uint32_t CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::Image* createdImage);
            uint32_t CreateTexture(stbi_uc* pixels, uint32_t width, uint32_t height, vk::Image* createdImage);
            vk::UniqueImageView CreateTextureView(const vk::Image& image);
            vk::UniqueSampler CreateTextureSampler();

            void FreeCommandBuffers(std::vector<vk::CommandBuffer>* commandBuffers);
            void DestroyBuffer(uint32_t id);
            void DestroyImage(uint32_t id);
            void MapMemory(uint32_t bufferId, std::vector<Vertex> vertices, size_t size);
            void MapMemory(uint32_t bufferId, std::vector<uint32_t> indices, size_t size);
            const SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
            void QueryDepthFormatSupport();
            void InitializeImgui(const vk::RenderPass& defaultPass);
            void TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

        private:
            void CreateInstance();
            void CreateSurface(HWND hwnd, HINSTANCE hinstance);
            void CreateAllocator();
            void CreateCommandPool();
            void CreateLogicalDevice();
            void CreatePhysicalDevice();
            void CreateCommandQueues();
            void CreateDescriptorPools();

            vk::Instance m_instance;
            vk::SurfaceKHR m_surface;
            vk::Device m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;
            vk::Format m_depthFormat;

            // @todo: The below resources don't need to live here, they should be managed by some resource management class.
            uint32_t m_bufferIndex;
            std::unordered_map<uint32_t, std::pair<vk::Buffer, vma::Allocation>> m_buffers;
            uint32_t m_imageIndex;
            std::unordered_map<uint32_t, std::pair<vk::Image, vma::Allocation>> m_images;
            vma::Allocator m_allocator;
            vk::CommandPool m_commandPool;
            vk::DescriptorPool m_imguiDescriptorPool;
            vk::DescriptorPool m_renderingDescriptorPool;
    };
}