#include "Engine.h"
#include "config/Config.h"
#include "utility/NcError.h"
#include "QueueFamily.h"
#include "Swapchain.h"

#include <set>

namespace
{
constexpr auto DeviceExtensions = std::array<const char*, 1>{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
constexpr auto GlobalExtensions = std::array<const char*, 2>{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
constexpr auto ValidationLayers = std::array<const char*, 1>{ "VK_LAYER_KHRONOS_validation" };

bool CheckValidationLayerSupport();
void SetValidationLayersEnabled(bool isEnabled, vk::InstanceCreateInfo& instanceCreateInfo);
void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo);

void SetValidationLayersEnabled(bool isEnabled, vk::InstanceCreateInfo& instanceCreateInfo)
{
        if (isEnabled)
    {
        if (!CheckValidationLayerSupport())
        {
            throw nc::NcError("Validation layers requested but not available.");
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
    }
    else 
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }
}

bool CheckValidationLayerSupport()
{
    uint32_t layerCount;
    if (vk::enumerateInstanceLayerProperties(&layerCount, nullptr) != vk::Result::eSuccess)
    {
        return false;
    }

    std::vector<vk::LayerProperties> availableLayers(layerCount);
    if (vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()) != vk::Result::eSuccess)
    {
        return false;
    }

    return std::all_of(ValidationLayers.cbegin(), ValidationLayers.cend(), [&availableLayers](const auto& requiredLayer)
    {
        return std::any_of(availableLayers.cbegin(), availableLayers.cend(), [&requiredLayer](const auto& availableLayer)
        {
            return strcmp(requiredLayer, availableLayer.layerName) == 0;
        });
    });
}

void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo)
{
    instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(GlobalExtensions.size()));
    instanceCreateInfo.setPpEnabledExtensionNames(GlobalExtensions.data());
}

vk::UniqueInstance CreateInstance()
{
    vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
    vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

    SetValidationLayersEnabled(nc::config::GetGraphicsSettings().useValidationLayers, instanceCreateInfo);
    SetGlobalExtensions(instanceCreateInfo);

    return createInstanceUnique(instanceCreateInfo);
}

vk::UniqueSurfaceKHR CreateSurface(HWND hwnd, HINSTANCE hinstance, vk::Instance instance)
{
    vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.setHinstance(hinstance);
    surfaceCreateInfo.setHwnd(hwnd);
    return instance.createWin32SurfaceKHRUnique(surfaceCreateInfo);
}

vk::PhysicalDevice CreatePhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface)
{
    vk::PhysicalDevice physicalDevice;

    uint32_t deviceCount = 0;
    if (instance.enumeratePhysicalDevices(&deviceCount, nullptr) != vk::Result::eSuccess)
    {
        throw nc::NcError("Count physical devices - Failed to find GPU that supports Vulkan.");
    }

    std::vector<vk::PhysicalDevice> devices(deviceCount);
    if (instance.enumeratePhysicalDevices(&deviceCount, devices.data()) != vk::Result::eSuccess)
    {
        throw nc::NcError("Get physical devices - Failed to find GPU that supports Vulkan.");
    }

    // Select the first suitable physical device
    bool foundSuitableDevice = false;
    for (const auto& device : devices)
    {
        auto indices = nc::graphics::QueueFamilyIndices(device, surface);

        uint32_t extensionCount;
        if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess)
        {
            throw nc::NcError("Could not enumerate device extensions.");
        }

        std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
        if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
        {
            throw nc::NcError("Could not enumerate device extensions.");
        }

        bool extensionsSupported = std::all_of(DeviceExtensions.cbegin(), DeviceExtensions.cend(), [&availableExtensions](const auto& requiredExtension)
        {
            return std::any_of(availableExtensions.cbegin(), availableExtensions.cend(), [&requiredExtension](const auto& availableExtension)
            {
                return strcmp(availableExtension.extensionName, requiredExtension) == 0;
            });
        });

        vk::PhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
        indexingFeatures.setPNext(nullptr);

        vk::PhysicalDeviceFeatures2 deviceFeatures{};
        deviceFeatures.setPNext(&indexingFeatures);
        device.getFeatures2(&deviceFeatures);

        if(!indexingFeatures.descriptorBindingPartiallyBound || !indexingFeatures.runtimeDescriptorArray)
        {
            continue;
        }

        bool swapChainAdequate = false;

        if (extensionsSupported)
        {
            auto swapChainSupportDetails = nc::graphics::QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupportDetails.formats.empty() && !swapChainSupportDetails.presentModes.empty();
        }

        if (indices.IsComplete() && extensionsSupported && swapChainAdequate)
        {
            physicalDevice = device;
            foundSuitableDevice = true;
            break;
        }
    }

    if (!foundSuitableDevice)
    {
        throw nc::NcError("Test physical devices for suitability - Failed to find GPU that supports Vulkan.");
    }

    return physicalDevice;
}

vk::UniqueDevice CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
    auto indices = nc::graphics::QueueFamilyIndices(physicalDevice, surface);

    std::set<uint32_t> uniqueQueueFamilies = {};

    uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(nc::graphics::QueueFamilyType::GraphicsFamily));

    if (indices.IsSeparatePresentQueue())
    {
        uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(nc::graphics::QueueFamilyType::PresentFamily));
    }

    const auto queuePriority = std::array<float, 1>{1.0f};
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::transform(uniqueQueueFamilies.cbegin(), uniqueQueueFamilies.cend(), std::back_inserter(queueCreateInfos), [&queuePriority](auto queueFamily) 
    {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.setQueueFamilyIndex(queueFamily);
        queueCreateInfo.setQueueCount(1);
        queueCreateInfo.setPQueuePriorities(queuePriority.data());
        return queueCreateInfo;
    });

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(VK_TRUE);
    deviceFeatures.setFillModeNonSolid(VK_TRUE);
    deviceFeatures.setWideLines(VK_TRUE);

    vk::PhysicalDeviceVulkan11Features vulkan11Features{};
    vulkan11Features.setShaderDrawParameters(VK_TRUE);

    vk::PhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
    indexingFeatures.setPNext(&vulkan11Features);
    indexingFeatures.setDescriptorBindingPartiallyBound(VK_TRUE);
    indexingFeatures.setRuntimeDescriptorArray(VK_TRUE);

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.setPNext(&indexingFeatures);
    deviceCreateInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
    deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
    deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);
    deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(DeviceExtensions.size()));
    deviceCreateInfo.setPpEnabledExtensionNames(DeviceExtensions.data());
    deviceCreateInfo.setEnabledLayerCount(0);

    return physicalDevice.createDeviceUnique(deviceCreateInfo);
}
}

namespace nc::graphics
{
Engine::Engine(HWND hwnd, HINSTANCE hinstance)
    : instance{CreateInstance()},
      surface{CreateSurface(hwnd, hinstance, instance.get())},
      physicalDevice{CreatePhysicalDevice(instance.get(), surface.get())},
      logicalDevice{CreateLogicalDevice(physicalDevice, surface.get())}
{
}
}