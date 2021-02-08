#include "Device.h"
#include "Instance.h"

#include <set>
#include <string>
#include <algorithm>

namespace
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface);
    const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

namespace nc::graphics::vulkan
{
    Device::Device(const vulkan::Instance* instance, Vector2 dimensions)
    : m_device{},
      m_physicalDevice{},
      m_graphicsQueue{},
      m_presentQueue{},
      m_swapChain{},
      m_swapChainImages{},
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
            auto indices = QueueFamilyIndices(device, vkSurface);

            uint32_t extensionCount;
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Device::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
            }

            std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Device::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
            }

            bool extensionsSupported = std::all_of(DeviceExtensions.cbegin(), DeviceExtensions.cend(), [&availableExtensions](const auto& requiredExtension)
            {
                return std::any_of(availableExtensions.cbegin(), availableExtensions.cend(), [&requiredExtension](const auto& availableExtension)
                {
                    return strcmp(availableExtension.extensionName, requiredExtension) == 0;
                });
            });

            bool swapChainAdequate = false;

            if (extensionsSupported)
            {
                auto swapChainSupportDetails = QuerySwapChainSupport(device, vkSurface);
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
        auto indices = QueueFamilyIndices(m_physicalDevice, vkSurface);

        std::set<uint32_t> uniqueQueueFamilies = {};

        uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily));

        if (indices.IsSeparatePresentQueue())
        {
            uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(QueueFamilyType::PresentFamily));
        }

        float queuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::transform(uniqueQueueFamilies.cbegin(), uniqueQueueFamilies.cend(), std::back_inserter(queueCreateInfos), [queuePriority](auto queueFamily) 
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.setQueueFamilyIndex(queueFamily);
            queueCreateInfo.setQueueCount(1);
            queueCreateInfo.setPQueuePriorities(&queuePriority);
            return queueCreateInfo;
        });

        /******************
         * LOGICAL DEVICE *
         * ****************/
        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
        deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
        deviceCreateInfo.setPEnabledFeatures(nullptr);
        deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(DeviceExtensions.size()));
        deviceCreateInfo.setPpEnabledExtensionNames(DeviceExtensions.data());
        deviceCreateInfo.setEnabledLayerCount(0);

        try
        {
            m_device = m_physicalDevice.createDevice(deviceCreateInfo);
        }
        catch (const std::exception& error)
        {
            std::throw_with_nested(std::runtime_error("Failed to create device."));
        }

        /**************
         * SWAP CHAIN *
         * ************/
        auto swapChainSupport = QuerySwapChainSupport(m_physicalDevice, vkSurface);

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

        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, vkSurface);
        uint32_t queueFamilyIndices[] = { queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily) };

        if (queueFamilies.IsSeparatePresentQueue())
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
        if (m_device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error getting swapchain images count.");
        }

        m_swapChainImages.resize(swapChainImageCount);
        if (m_device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, m_swapChainImages.data()) != vk::Result::eSuccess)
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

        for (size_t i = 0; i < m_swapChainImages.size(); ++i)
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

    QueueFamilyIndices::QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface)
    {
        uint32_t queueFamilyCount = 0;
        device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

        std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
        device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());
        
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            vk::Bool32 presentSupport = false;
            if (device.getSurfaceSupportKHR(i, *surface, &presentSupport) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Could not get surface support KHR");
            }

            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                m_graphicsFamily = i;
            }

            if (presentSupport)
            {
                m_presentFamily = i;
            }

            if (IsComplete())
            {
                break;
            }

            ++i;
        }

        if (!m_graphicsFamily.has_value())
        {
            throw std::runtime_error("No graphics queue family found on device.");
        }

        if (!m_presentFamily.has_value())
        {
            throw std::runtime_error("No graphics queue family found on device.");
        }

        m_isSeparatePresentQueue = m_presentFamily != m_graphicsFamily ? true : false;
    }

    bool QueueFamilyIndices::IsComplete() const
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }

    bool QueueFamilyIndices::IsSeparatePresentQueue() const
    {
        return m_isSeparatePresentQueue;
    }

    uint32_t QueueFamilyIndices::GetQueueFamilyIndex(QueueFamilyType type) const
    {
        if (!IsComplete())
        {
            throw std::runtime_error("QueueFamilyIndices::GetQueueFamilyIndex() - QueueFamilies incomplete.");
        }

        switch (type)
        {
            case QueueFamilyType::GraphicsFamily:
                return m_graphicsFamily.value();
            case QueueFamilyType::PresentFamily:
                return m_presentFamily.value();
        }
        throw std::runtime_error("QueueFamilyIndices::GetQueueFamilyIndex() - Chosen queue not present.");
    }
}

namespace
{
    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR* surface)
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
}