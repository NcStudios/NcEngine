#include "VkUtility.h"
#include "debug/NcError.h"

namespace nc::graphics
{
    auto QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) -> SwapChainSupportDetails
    {
        SwapChainSupportDetails details;
        if (device.getSurfaceCapabilitiesKHR(surface, &details.capabilities) != vk::Result::eSuccess)
        {
            throw NcError("SwapChain::QuerySwapChainSupport() - Could not enumerate surface capabilities.");
        }

        uint32_t formatCount;
        if (device.getSurfaceFormatsKHR(surface, &formatCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
        }

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            if (device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data()) != vk::Result::eSuccess)
            {
                throw NcError("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
            }
        }

        uint32_t presentModeCount;
        if (device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
        }

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            if (device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data()) != vk::Result::eSuccess)
            {
                throw NcError("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
            }
        }

        return details;
    }

    auto PadBufferOffsetAlignment(uint32_t originalSize, vk::DescriptorType bufferType, const vk::PhysicalDeviceProperties& deviceProperties) -> uint32_t
    {
        uint32_t minimumAlignment = 0;

        switch (bufferType)
        {
            case vk::DescriptorType::eStorageBuffer:
            {
                minimumAlignment = static_cast<uint32_t>(deviceProperties.limits.minStorageBufferOffsetAlignment);
                break;
            }
            case vk::DescriptorType::eUniformBuffer:
            {
                minimumAlignment = static_cast<uint32_t>(deviceProperties.limits.minUniformBufferOffsetAlignment);
                break;
            }
            default:
            {
                throw NcError("Invalid bufferType chosen.");
            }
        }

        uint32_t alignedSize = originalSize;

        if (minimumAlignment > 0)
        {
            alignedSize = (alignedSize + minimumAlignment - 1) & ~(minimumAlignment - 1);
        }
        return alignedSize;
    }
}