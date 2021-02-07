#include "Device.h"
#include "Instance.h"

#include <set>
#include <string>

namespace nc::graphics::vulkan
{
    Device::Device(const vulkan::Instance* instance, Vector2 dimensions)
    : m_device{},
      m_physicalDevice{},
      m_graphicsQueue{},
      m_presentQueue{},
      m_swapChain{},
      m_swapChainImageFormat{},
      m_swapChainExtent{},
      m_swapChainImageViews{},
      m_dimensions{dimensions}
    {
        auto vkInstance = instance->GetInstance();
        auto vkSurface = instance->GetSurface();

        /*******************
         * PHYSICAL DEVICE *
         * *****************/
        uint32_t deviceCount = 0;
        if (vkInstance->enumeratePhysicalDevices(&deviceCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Count physical devices - Failed to find GPU that supports Vulkan.");
        }

        std::vector<vk::PhysicalDevice> devices(deviceCount);
        if (vkInstance->enumeratePhysicalDevices(&deviceCount, devices.data()) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Get physical devices - Failed to find GPU that supports Vulkan.");
        }

        // Select the first suitable physical device
        bool foundSuitableDevice = false;
        for (const auto& device : devices)
        {
            QueueFamilyIndices indices = Device::FindQueueFamilies(device, vkSurface);

            uint32_t extensionCount;
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Device::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
            }

            bool extensionsSupported;
            std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Device::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
            }

            for (const char* extensionName : m_deviceExtensions)
            {
                bool extensionFound = false;

                for (const auto& availableExtension : availableExtensions)
                {
                    if (strcmp(extensionName, availableExtension.extensionName) == 0)
                    {
                        extensionFound = true;
                        break;
                    }
                }

                if (!extensionFound)
                {
                    extensionsSupported = false;
                }
            }
            extensionsSupported = true;

            bool swapChainAdequate = false;

            if (extensionsSupported)
            {
                auto swapChainSupportDetails = Device::QuerySwapChainSupport(device, vkSurface);
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
            throw std::runtime_error("Test physical devices for suitability - Failed to find GPU that supports Vulkan.");
        }

        /******************
         * COMMAND QUEUES *
         * ****************/
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice, vkSurface);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {};

        uniqueQueueFamilies.emplace(indices.graphicsFamily.value());

        if (indices.isSeparatePresentQueue)
        {
            uniqueQueueFamilies.emplace(indices.presentFamily.value());
        }

        float queuePriority = 1.0f;
        for (auto queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.setQueueFamilyIndex(queueFamily);
            queueCreateInfo.setQueueCount(1);
            queueCreateInfo.setPQueuePriorities(&queuePriority);

            queueCreateInfos.push_back(queueCreateInfo);
        }

        /******************
         * LOGICAL DEVICE *
         * ****************/
        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
        deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
        deviceCreateInfo.setPEnabledFeatures(nullptr);
        deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_deviceExtensions.size()));
        deviceCreateInfo.setPpEnabledExtensionNames(m_deviceExtensions.data());
        deviceCreateInfo.setEnabledLayerCount(0);

        try
        {
            m_device = m_physicalDevice.createDevice(deviceCreateInfo);
        }
        catch (std::exception& error)
        {
            std::string message = "Device::Create() - Failed to create device: ";
            message.append(error.what());
            throw std::runtime_error(message);
        }

        /**************
         * SWAP CHAIN *
         * ************/
        auto swapChainSupport = Device::QuerySwapChainSupport(m_physicalDevice, vkSurface);

        auto surfaceFormat = swapChainSupport.formats[0];
        for (const auto& availableFormat : swapChainSupport.formats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                surfaceFormat = availableFormat;
                break;
            }
        }

        auto presentMode = vk::PresentModeKHR::eFifo;
        for (const auto& availablePresentMode : swapChainSupport.presentModes)
        {
            // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPresentModeKHR.html
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                presentMode = availablePresentMode;
                break;
            }
        }

        vk::Extent2D extent;
        if (swapChainSupport.capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = swapChainSupport.capabilities.currentExtent;
        }
        else
        {
            vk::Extent2D actualExtent = 
            {
                static_cast<uint32_t>(m_dimensions.x),
                static_cast<uint32_t>(m_dimensions.y)
            };

            actualExtent.width = std::max(swapChainSupport.capabilities.minImageExtent.width, std::min(swapChainSupport.capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(swapChainSupport.capabilities.minImageExtent.height, std::min(swapChainSupport.capabilities.maxImageExtent.height, actualExtent.height));

            extent = actualExtent;
        }

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(*vkSurface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        auto queueFamilies = FindQueueFamilies(m_physicalDevice, vkSurface);
        uint32_t queueFamilyIndices[] = {queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};

        if (queueFamilies.graphicsFamily != queueFamilies.presentFamily)
        {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        m_swapChain = m_device.createSwapchainKHR(createInfo);

        // Set swapchain images        
        uint32_t swapChainImageCount;
        auto countResult = m_device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, nullptr);
        if (countResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error getting swapchain images count.");
        }

        auto result = m_device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, m_swapChainImages.data());
        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error getting swapchain images.");
        }

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;

        // Create image views
        m_swapChainImageViews.resize(m_swapChainImages.size());

        auto swapChainComponents = vk::ComponentMapping{};
        swapChainComponents.setR(vk::ComponentSwizzle::eIdentity);
        swapChainComponents.setG(vk::ComponentSwizzle::eIdentity);
        swapChainComponents.setB(vk::ComponentSwizzle::eIdentity);
        swapChainComponents.setA(vk::ComponentSwizzle::eIdentity);

        auto swapChainSubresourceRange = vk::ImageSubresourceRange{};
        swapChainSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        swapChainSubresourceRange.setBaseMipLevel(0);
        swapChainSubresourceRange.setLevelCount(1);
        swapChainSubresourceRange.setBaseArrayLayer(0);
        swapChainSubresourceRange.setLayerCount(1);

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            vk::ImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.setImage(m_swapChainImages[i]);
            imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
            imageViewCreateInfo.setFormat(m_swapChainImageFormat);
            imageViewCreateInfo.setComponents(swapChainComponents);
            imageViewCreateInfo.setSubresourceRange(swapChainSubresourceRange);

            if (m_device.createImageView(&imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Failed to create image view");
            }
        }
    }

    Device::~Device()
    {
        for (auto imageView : m_swapChainImageViews)
        {
            m_device.destroyImageView(imageView, nullptr);
        }

        m_device.destroySwapchainKHR(m_swapChain);
        m_device.destroy();
    }

    SwapChainSupportDetails Device::QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface)
    {
        SwapChainSupportDetails details;
        if (device.getSurfaceCapabilitiesKHR(*surface, &details.capabilities) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface capabilities.");
        }

        uint32_t formatCount;
        if (device.getSurfaceFormatsKHR(*surface, &formatCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
        }

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            if (device.getSurfaceFormatsKHR(*surface, &formatCount, details.formats.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
            }
        }

        uint32_t presentModeCount;
        if (device.getSurfacePresentModesKHR(*surface, &presentModeCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
        }

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            if (device.getSurfacePresentModesKHR(*surface, &presentModeCount, details.presentModes.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
            }
        } 
        return details;
    }

    QueueFamilyIndices Device::FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface)
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

        std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
        device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());
        
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            vk::Bool32 presentSupport = false;
            if (device.getSurfaceSupportKHR(i, *surface, &presentSupport) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Could not get surface support KHR");
            }
            
            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }

            i++;
        }

        indices.isSeparatePresentQueue = (indices.presentFamily != indices.graphicsFamily || !indices.presentFamily.has_value()) ? true : false;
        return indices;
    }
}