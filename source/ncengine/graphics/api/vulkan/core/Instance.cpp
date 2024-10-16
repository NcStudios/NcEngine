#include "Instance.h"

#include "GLFW/glfw3.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>

namespace
{
constexpr auto g_validationLayers = std::array<const char*, 1>{ "VK_LAYER_KHRONOS_validation" };

auto CheckValidationLayerSupport() -> bool
{
    auto layerCount = uint32_t{};
    if (vk::enumerateInstanceLayerProperties(&layerCount, nullptr) != vk::Result::eSuccess)
    {
        return false;
    }

    auto availableLayers = std::vector<vk::LayerProperties>(layerCount);
    if (vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()) != vk::Result::eSuccess)
    {
        return false;
    }

    return std::ranges::all_of(g_validationLayers, [&availableLayers](const auto& requiredLayer)
    {
        return std::ranges::any_of(availableLayers, [&requiredLayer](const auto& availableLayer)
        {
            return strcmp(requiredLayer, availableLayer.layerName) == 0;
        });
    });
}

void SetValidationLayersEnabled(bool isEnabled, vk::InstanceCreateInfo& instanceCreateInfo)
{
    if (isEnabled)
    {
        if (!::CheckValidationLayerSupport())
        {
            throw nc::NcError("Validation layers requested but not available.");
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = g_validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }
}

void SetExtensions(vk::InstanceCreateInfo& instanceCreateInfo)
{
    auto glfwExtensionsCount = 0u;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    instanceCreateInfo.setEnabledExtensionCount(glfwExtensionsCount);
    instanceCreateInfo.setPpEnabledExtensionNames(glfwExtensions);
}

auto CreateInstance(std::string_view appName, uint32_t appVersion,
                    uint32_t apiVersion, bool enableValidationLayers) -> vk::UniqueInstance
{
    constexpr auto engineName = "NcEngine";
    constexpr auto engineVersion = 1;
    auto appInfo = vk::ApplicationInfo{
        appName.data(),
        appVersion,
        engineName,
        engineVersion,
        apiVersion
    };
    auto createInfo = vk::InstanceCreateInfo{{}, &appInfo};
    ::SetValidationLayersEnabled(enableValidationLayers, createInfo);
    ::SetExtensions(createInfo);

    return vk::createInstanceUnique(createInfo);
}
} // anonymous namespace

namespace nc::graphics::vulkan
{
Instance::Instance(std::string_view appName, uint32_t appVersion,
                   uint32_t apiVersion, bool enableValidationLayers)
    : m_instance{::CreateInstance(appName, appVersion, apiVersion, enableValidationLayers)}
{
}

auto Instance::CreateSurface(GLFWwindow* window) const -> vk::UniqueSurfaceKHR
{
    VkSurfaceKHR surface;
    auto result = glfwCreateWindowSurface(m_instance.get(), window, nullptr, &surface);
    if (result != VkResult::VK_SUCCESS)
    {
        throw nc::NcError("Creating a surface - Failed to create a surface.");
    }
    return vk::UniqueSurfaceKHR(surface, m_instance.get());
}

auto Instance::GetPhysicalDevices() const -> std::vector<vk::PhysicalDevice>
{
    auto deviceCount = uint32_t{};
    if (m_instance->enumeratePhysicalDevices(&deviceCount, nullptr) != vk::Result::eSuccess)
    {
        throw nc::NcError("Count physical devices - Failed to find GPU that supports Vulkan.");
    }

    auto devices = std::vector<vk::PhysicalDevice>(deviceCount);
    if (m_instance->enumeratePhysicalDevices(&deviceCount, devices.data()) != vk::Result::eSuccess)
    {
        throw nc::NcError("Get physical devices - Failed to find GPU that supports Vulkan.");
    }
    return devices;
}
} // namespace nc::graphics::vulkan
