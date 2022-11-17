#pragma once

#include "math/Vector.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    auto QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) -> SwapChainSupportDetails;

    class PerFrameGpuContext;

    class Swapchain
    {
        public:
            Swapchain(vk::Device device, vk::PhysicalDevice physicalDevice,
                      vk::SurfaceKHR surface, const Vector2& dimensions);
            ~Swapchain() noexcept;

            // Swap chain
            void Present(PerFrameGpuContext* currentFrame, vk::Queue queue, uint32_t imageIndex, bool& isSwapChainValid);
            void Create(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, Vector2 dimensions);
            void Cleanup() noexcept;
            const Vector2 GetExtentDimensions() const noexcept;
            const vk::Extent2D& GetExtent() const noexcept;
            const vk::Format& GetFormat() const noexcept;
            const std::vector<vk::ImageView>& GetColorImageViews() const noexcept;
            vk::ImageView& GetDepthView() const noexcept;

            // Image synchronization
            bool GetNextRenderReadyImageIndex(PerFrameGpuContext* currentFrame, uint32_t* imageIndex);
            void WaitForNextImage(PerFrameGpuContext* currentFrame, uint32_t imageIndex);

        private:
            vk::Device m_device;
            vk::SwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapChainImages;
            vk::Format m_swapChainImageFormat;
            vk::Extent2D m_swapChainExtent;
            std::vector<vk::ImageView> m_swapChainImageViews;
            std::vector<vk::Fence> m_imagesInFlightFences;
    };
} // namespace nc::graphics
