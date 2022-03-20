#include "Device.h"
#include "debug/NcError.h"
#include "QueueFamily.h"
#include "VkUtility.h"

#include <array>

namespace
{
    const std::array<const char*, 1> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const std::array<const char*, 2> GlobalExtensions = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
    const std::array<const char*, 1> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    #ifdef NC_VULKAN_VALIDATION_ENABLED
        const bool IsValidationLayersEnabled = true;
    #else
        const bool IsValidationLayersEnabled = false;
    #endif

    void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo)
    {
        if (IsValidationLayersEnabled)
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

    auto CreateInstance() -> vk::Instance
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        EnableValidationLayers(instanceCreateInfo);
        SetGlobalExtensions(instanceCreateInfo);

        try
        {
            return createInstance(instanceCreateInfo);
        }
        catch (const std::exception&)
        {
            std::throw_with_nested(nc::NcError("Failed to create instance."));
        }
    }

    auto CreateSurface(HWND hwnd, HINSTANCE hinstance, vk::Instance* instance)
    {
        vk::SurfaceKHR surface;
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.setHinstance(hinstance);
        surfaceCreateInfo.setHwnd(hwnd);
        if (instance->createWin32SurfaceKHR(&surfaceCreateInfo, nullptr, &surface) != vk::Result::eSuccess)
        {
            throw nc::NcError("Failed to get surface.");
        }

        return surface;
    }
}

namespace nc::graphics
{
    Device::Device(HWND hwnd, HINSTANCE hinstance)
        : m_instance{CreateInstance()},
          m_surface{CreateSurface(hwnd, hinstance, &m_instance)}
    {
        CreatePhysicalDevice();
    }

    void Device::CreatePhysicalDevice()
    {
        uint32_t deviceCount = 0;
        if (m_instance.enumeratePhysicalDevices(&deviceCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Count physical devices - Failed to find GPU that supports Vulkan.");
        }

        std::vector<vk::PhysicalDevice> devices(deviceCount);
        if (m_instance.enumeratePhysicalDevices(&deviceCount, devices.data()) != vk::Result::eSuccess)
        {
            throw NcError("Get physical devices - Failed to find GPU that supports Vulkan.");
        }

        // Select the first suitable physical device
        bool foundSuitableDevice = false;
        for (const auto& device : devices)
        {
            auto indices = QueueFamilyIndices(device, m_surface);

            uint32_t extensionCount;
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess)
            {
                throw NcError("Could not enumerate device extensions.");
            }

            std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
            {
                throw NcError("Could not enumerate device extensions.");
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
                auto swapChainSupportDetails = QuerySwapChainSupport(m_physicalDevice, m_surface);
                swapChainAdequate = !swapChainSupportDetails.formats.empty() && !swapChainSupportDetails.presentModes.empty();
            }

            if (indices.IsComplete() && extensionsSupported && swapChainAdequate)
            {
                m_physicalDevice = device;
                foundSuitableDevice = true;
                break;
            }
        }

        if (!foundSuitableDevice)
        {
            throw NcError("Test physical devices for suitability - Failed to find GPU that supports Vulkan.");
        }
    }
}