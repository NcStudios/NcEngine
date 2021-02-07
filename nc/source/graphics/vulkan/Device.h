#pragma once

#include "math/Vector2.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Instance;

    static const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    enum class QueueFamilyType : uint8_t
    {
        GraphicsFamily,
        PresentFamily
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isSeparatePresentQueue;

        bool IsComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }

        uint32_t GetQueueFamilyIndex(QueueFamilyType type) const
        {
            if (!IsComplete())
            {
                throw std::runtime_error("QueueFamilyIndices::GetQueueFamilyIndex() - QueueFamilies incomplete.");
            }

            switch (type)
            {
                case QueueFamilyType::GraphicsFamily:
                    return graphicsFamily.value();
                case QueueFamilyType::PresentFamily:
                    if (isSeparatePresentQueue)
                    {   
                        return presentFamily.value();
                    }
                    return graphicsFamily.value();
            }
            throw std::runtime_error("QueueFamilyIndices::GetQueueFamilyIndex() - Chosen queue not present.");
        }
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class Device
    {
        public:
            Device(const vulkan::Instance* instance, Vector2 dimensions);
            ~Device();

        private:
            vk::Device m_device;
            vk::PhysicalDevice m_physicalDevice;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;
            vk::SwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapChainImages;
            vk::Format m_swapChainImageFormat;
            vk::Extent2D m_swapChainExtent;
            std::vector<vk::ImageView> m_swapChainImageViews;
            Vector2 m_dimensions;

            SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface);
            QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface);
    };
}