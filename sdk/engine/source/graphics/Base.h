#pragma once

#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "imgui/imgui_impl_vulkan.h"
#include "GpuAllocator.h"

#include <optional>
#include <unordered_map>
#include <vector>

typedef unsigned char stbi_uc;

namespace nc::graphics
{
    class Instance;
    class Commands;
    class TexturesData;
    struct Vertex;

    // How many frames can be rendered concurrently.
    // Each frame requires its own pair of semaphores.
    constexpr uint32_t MaxFramesInFlight = 2u;

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
            ~Base() noexcept;

            const vk::Device& GetDevice() const noexcept;
            const vk::PhysicalDevice& GetPhysicalDevice() const noexcept;
            const vk::Instance& GetInstance() const noexcept;
            const vk::SurfaceKHR& GetSurface() const noexcept;
            const vk::CommandPool& GetCommandPool() const noexcept;
            const vk::Queue& GetQueue(QueueFamilyType type) const noexcept;
            const vk::Format& GetDepthFormat() const noexcept;
            vk::SampleCountFlagBits GetMaxSamplesCount();

            vk::UniqueSampler CreateTextureSampler();

            void FreeCommandBuffers(std::vector<vk::CommandBuffer>* commandBuffers) noexcept;
            const SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
            void QueryDepthFormatSupport();
            void InitializeImgui(const vk::RenderPass& defaultPass);
            void TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, uint32_t layerCount, vk::ImageLayout newLayout);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
            void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);

        private:
            void CreateInstance();
            void CreateSurface(HWND hwnd, HINSTANCE hinstance);
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
            vk::SampleCountFlagBits m_samplesCount;
            bool m_samplesInitialized;

            vk::CommandPool m_commandPool;
            vk::DescriptorPool m_imguiDescriptorPool;
            vk::PhysicalDeviceProperties m_gpuProperties;
    };
}