#pragma once

#include "graphics/api/vulkan/NcVulkan.h"

#include "ncmath/Vector.h"

#include <span>

namespace nc::graphics::vulkan
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    auto QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) -> SwapChainSupportDetails;

    class Device;
    class PerFrameGpuContext;
    class Swapchain
    {
        public:
            Swapchain(const Device& device, vk::SurfaceKHR surface, const Vector2& dimensions);
            ~Swapchain() noexcept;

            // Swap chain
            auto PresentImageToSwapChain(PerFrameGpuContext* currentFrame, vk::Queue queue, uint32_t imageIndex) -> bool;
            void Cleanup() noexcept;
            void Resize(const Device& device, const Vector2& dimensions);

            auto GetExtent() const noexcept -> const vk::Extent2D &;
            auto GetFormat() const noexcept -> const vk::Format&;
            auto GetSwapchainImageViews() const noexcept -> std::span<const vk::ImageView>;

            // Image synchronization
            bool GetNextRenderReadyImageIndex(PerFrameGpuContext* currentFrame, uint32_t* imageIndex);
            void WaitImageReadyForBuffer(PerFrameGpuContext* currentFrame, uint32_t imageIndex);

        private:
            void Create(const Device& device, vk::SurfaceKHR surface, Vector2 dimensions);

            vk::Device m_device;
            vk::SurfaceKHR m_surface;
            vk::UniqueSwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapChainImages;
            vk::Format m_swapChainImageFormat;
            vk::Extent2D m_swapChainExtent;
            std::vector<vk::ImageView> m_swapChainImageViews;
            std::vector<vk::Fence> m_imagesInFlightFences;
    };
} // namespace nc::graphics::vulkan
