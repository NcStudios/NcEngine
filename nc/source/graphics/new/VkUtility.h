#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc::graphics
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    auto QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) -> SwapChainSupportDetails;

    auto PadBufferOffsetAlignment(uint32_t originalSize, vk::DescriptorType bufferType, const vk::PhysicalDeviceProperties& deviceProperties) -> uint32_t;

}