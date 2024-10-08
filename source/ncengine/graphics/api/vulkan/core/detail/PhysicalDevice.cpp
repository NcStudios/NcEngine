#include "PhysicalDevice.h"
#include "graphics/api/vulkan/Swapchain.h"

#include "ncutility/NcError.h"

#include <algorithm>

namespace
{
auto DeviceSupportsExtensions(vk::PhysicalDevice physicalDevice, std::span<const char* const> requiredExtensions) -> bool
{
    auto extensionCount = uint32_t{};
    if (physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess)
    {
        throw nc::NcError("Could not enumerate device extensions.");
    }

    auto availableExtensions = std::vector<vk::ExtensionProperties>(extensionCount);
    if (physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
    {
        throw nc::NcError("Could not enumerate device extensions.");
    }

    return std::ranges::all_of(requiredExtensions, [&availableExtensions](auto&& requiredName)
    {
        return std::ranges::any_of(availableExtensions, [&requiredName](auto&& availableExtension)
        {
            return strcmp(availableExtension.extensionName, requiredName) == 0;
        });
    });
}

auto DeviceSupportsFeatures(vk::PhysicalDevice physicalDevice) -> bool
{
    auto indexingFeatures = vk::PhysicalDeviceDescriptorIndexingFeaturesEXT{};
    indexingFeatures.setPNext(nullptr);
    auto deviceFeatures = vk::PhysicalDeviceFeatures2{};
    deviceFeatures.setPNext(&indexingFeatures);
    physicalDevice.getFeatures2(&deviceFeatures);

    return indexingFeatures.descriptorBindingPartiallyBound && indexingFeatures.runtimeDescriptorArray;
}
} // anonymous namespace

namespace nc::graphics::vulkan::detail
{
auto SelectPhysicalDevice(std::span<const vk::PhysicalDevice> physicalDevices,
                          vk::SurfaceKHR surface,
                          std::span<const char* const> requiredExtensions) -> PhysicalDeviceInfo
{
    for(auto&& device : physicalDevices)
    {
        if (!DeviceSupportsExtensions(device, requiredExtensions))
        {
            continue;
        }

        if (!DeviceSupportsFeatures(device))
        {
            continue;
        }

        auto swapChainSupportDetails = QuerySwapChainSupport(device, surface);
        if(swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty())
        {
            continue;
        }

        auto indices = QueueFamilyIndices{device, surface};
        if (!indices.IsComplete())
        {
            continue;
        }

        return PhysicalDeviceInfo{device, indices};
    }

    throw NcError("Could not find a suitable physical device.");
}
} // namespace nc::graphics::vulkan::detail
