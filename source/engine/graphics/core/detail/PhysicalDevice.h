#pragma once

#include "graphics/QueueFamily.h"

#include <span>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc::graphics::detail
{
struct PhysicalDeviceInfo
{
    vk::PhysicalDevice physicalDevice;
    QueueFamilyIndices queueFamilyIndices;
};

auto SelectPhysicalDevice(std::span<const vk::PhysicalDevice> physicalDevices,
                          vk::SurfaceKHR surface,
                          std::span<const char* const> requiredExtensions) -> PhysicalDeviceInfo;
} // namespace nc::graphics::detail
