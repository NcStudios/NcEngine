#include "Device.h"
#include "Instance.h"
#include "VertexBuffer.h"

#include <set>
#include <string>
#include <algorithm>

#define VMA_IMPLEMENTATION
#include "vulkan/vk_mem_alloc.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
    const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

namespace nc::graphics::vulkan
{
    Device::Device(const vulkan::Instance& instance, Vector2 dimensions)
    : m_instance{instance.GetInstance()},
      m_surface{instance.GetSurface()},
      m_device{},
      m_physicalDevice{},
      m_graphicsQueue{},
      m_presentQueue{},
      m_swapChain{},
      m_swapChainImages{},
      m_swapChainImageFormat{},
      m_swapChainExtent{},
      m_swapChainImageViews{},
      m_commandPool{},
      m_commandBuffers{},
      m_imageRenderReadySemaphores{},
      m_imagePresentReadySemaphores{},
      m_framesInFlightFences{},
      m_imagesInFlightFences{},
      m_currentFrameIndex{0},
      m_allocator{},
      m_buffers{},
      m_bufferIndex{0}
    {
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain(dimensions);
        CreateCommandPool();
        CreateSynchronizationObjects();
        CreateAllocator();
    }

    Device::~Device()
    {
        CleanupSwapChain();
        m_device.destroyCommandPool(m_commandPool);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_device.destroySemaphore(m_imageRenderReadySemaphores[i]);
            m_device.destroySemaphore(m_imagePresentReadySemaphores[i]);
            m_device.destroyFence(m_framesInFlightFences[i]);
        }

        for (uint32_t i = 0; i < m_buffers.size(); i++)
        {
            m_allocator.destroyBuffer(m_buffers[i].first, m_buffers[i].second);
        }

        m_allocator.destroy();
        m_device.destroy();
    }

    void Device::CleanupSwapChain()
    {
        for (auto imageView : m_swapChainImageViews)
        {
            m_device.destroyImageView(imageView, nullptr);
        }

        m_device.destroySwapchainKHR(m_swapChain);
    }

    void Device::CreatePhysicalDevice()
    {
        uint32_t deviceCount = 0;
        if (m_instance.enumeratePhysicalDevices(&deviceCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Count physical devices - Failed to find GPU that supports Vulkan.");
        }

        std::vector<vk::PhysicalDevice> devices(deviceCount);
        if (m_instance.enumeratePhysicalDevices(&deviceCount, devices.data()) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Get physical devices - Failed to find GPU that supports Vulkan.");
        }

        // Select the first suitable physical device
        bool foundSuitableDevice = false;
        for (const auto& device : devices)
        {
            auto indices = QueueFamilyIndices(device, m_surface);

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
                auto swapChainSupportDetails = QuerySwapChainSupport(device, m_surface);
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
    }

    void Device::CreateLogicalDevice()
    {
        auto indices = QueueFamilyIndices(m_physicalDevice, m_surface);

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
    }

    void Device::CreateSwapChain(Vector2 dimensions)
    {
        auto swapChainSupport = QuerySwapChainSupport(m_physicalDevice, m_surface);

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
                static_cast<uint32_t>(dimensions.x),
                static_cast<uint32_t>(dimensions.y)
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
        createInfo.setSurface(m_surface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        uint32_t queueFamilyIndices[] = { queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily) };

        m_graphicsQueue = m_device.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), 0);
        m_presentQueue = m_device.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily), 0);

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

    void Device::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo poolInfo{};
        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        poolInfo.setQueueFamilyIndex(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily));
        m_commandPool = m_device.createCommandPool(poolInfo);
    }

    // The semaphores deal solely with the GPU. Since rendering to an image taken from the swapchain and returning that image back to the swap chain are both asynchronous, 
    // the semaphores below tell the GPU when either step can begin for a single image. Vulkan will render multiple swapchain images very rapidly, so MAX_FRAMES_IN_FLIGHT here creates 
    // a pair of semaphores for each frame up to MAX_FRAMES_IN_FLIGHT. 
    // The fences synchronize GPU - CPU and they are what limit Vulkan to submitting only MAX_FRAMES_IN_FLIGHT amount of frame-render jobs to the command queues. 
    // The fences in framesInFlightFences (one per frame in MAX_FRAMES_PER_FLIGHT) prevent more frame-render jobs than fences from being submitted until one frame-render job completes.
    // The fences in imagesInFlightFences (one per swapchain image) track for each swap chain image whether it is being used by a frame in flight.
    void Device::CreateSynchronizationObjects()
    {
        m_imageRenderReadySemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_imagePresentReadySemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_framesInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_imagesInFlightFences.resize(m_swapChainImages.size(), nullptr); // To start, no frames in flight are using swapchain images, so explicitly initialize to nullptr.

        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_imageRenderReadySemaphores[i] = m_device.createSemaphore(semaphoreInfo);
            m_imagePresentReadySemaphores[i] = m_device.createSemaphore(semaphoreInfo);
            m_framesInFlightFences[i] = m_device.createFence(fenceInfo);
        }
    }

    uint32_t Device::CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, bool isStaging, vk::Buffer* createdBuffer)
    {
        if (m_buffers.find(m_bufferIndex) != m_buffers.end())
        {
            throw std::runtime_error("The given ID is already present in the dictionary.");
        }

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(size);
        bufferInfo.setUsage(usageFlags);

        vma::AllocationCreateInfo allocationInfo;
        allocationInfo.usage = isStaging ? vma::MemoryUsage::eCpuOnly : vma::MemoryUsage::eGpuOnly;

        vma::Allocation allocation;
        vk::Buffer buffer;
        if (m_allocator.createBuffer(&bufferInfo, &allocationInfo, &buffer, &allocation, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error creating buffer.");
        }

        m_buffers.emplace(m_bufferIndex, std::pair{buffer, allocation});
        *createdBuffer = buffer;
        return m_bufferIndex++;
    }

    void Device::DestroyBuffer(uint32_t id)
    {
        auto buffer = m_buffers.find(id);
        if (buffer == m_buffers.end())
        {
            throw std::runtime_error("The given ID was not present in the dictionary.");
        }

        m_allocator.destroyBuffer(buffer->second.first, buffer->second.second);
        m_buffers.erase(id);
    }

    void Device::CreateAllocator()
    {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorInfo.physicalDevice = m_physicalDevice;
        allocatorInfo.device = m_device;
        allocatorInfo.instance = m_instance;
        m_allocator = vma::createAllocator(allocatorInfo);
    }

    const vk::Device& Device::GetDevice() const noexcept
    {
        return m_device;
    }

    const Vector2 Device::GetSwapChainExtentDimensions() const noexcept
    {
        return Vector2(m_swapChainExtent.width, m_swapChainExtent.height);
    }

    const vk::Extent2D& Device::GetSwapChainExtent() const noexcept
    {
        return m_swapChainExtent;
    }

    const vk::Format& Device::GetSwapChainImageFormat() const noexcept
    {
        return m_swapChainImageFormat;
    }

    const std::vector<vk::ImageView>& Device::GetSwapChainImageViews() const noexcept
    {
        return m_swapChainImageViews;
    }

    uint32_t Device::GetNextRenderReadyImageIndex(bool& isSwapChainValid)
    {
        auto resultAndIndex = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, m_imageRenderReadySemaphores[m_currentFrameIndex]);
        if (resultAndIndex.result == vk::Result::eErrorOutOfDateKHR)
        {
            isSwapChainValid = false;
        }

        return resultAndIndex.value;
    }

    const std::vector<vk::Semaphore>& Device::GetSemaphores(SemaphoreType semaphoreType) const noexcept
    {
        return semaphoreType == SemaphoreType::RenderReady ? m_imageRenderReadySemaphores : m_imagePresentReadySemaphores;
    }

    const vk::CommandPool& Device::GetCommandPool() const noexcept
    {
        return m_commandPool;
    }

    const vk::Queue& Device::GetQueue(QueueFamilyType type) const noexcept
    {
        return type == QueueFamilyType::GraphicsFamily ? m_graphicsQueue : m_presentQueue;
    }

    uint32_t Device::GetFrameIndex() const noexcept
    {
        return m_currentFrameIndex;
    }

    const std::vector<vk::Fence>& Device::GetFences(FenceType fenceType) const noexcept
    {
        return fenceType == FenceType::FramesInFlight ? m_framesInFlightFences : m_imagesInFlightFences;
    }

    const vma::Allocation& Device::GetAllocation(uint32_t bufferId) const
    {
        return m_buffers.at(bufferId).second;
    }

    void Device::Present(uint32_t imageIndex, bool& isSwapChainValid)
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(&m_imagePresentReadySemaphores[m_currentFrameIndex]); // Wait on this semaphore before presenting.

        vk::SwapchainKHR swapChains[] = {m_swapChain};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains); // Sets the swapchain(s) to present to
        presentInfo.setPImageIndices(&imageIndex); //  Sets the index of the image for each swapchain.
        presentInfo.setPResults(nullptr);

        auto result = m_presentQueue.presentKHR(&presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            isSwapChainValid = false;
            return;
        }
        
        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Could not present to the swapchain.");
        }
    }

    // Increments the frame index. Frame index is used to select which pair of semaphores we are using as each concurrent frame requires its own pair.
    void Device::IncrementFrameIndex()
    {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Device::WaitForFrameFence()
    {
        if (m_device.waitForFences(m_framesInFlightFences[m_currentFrameIndex], true, UINT64_MAX) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Could not wait for fences to complete.");
        }
    }

    void Device::WaitForImageFence(uint32_t imageIndex)
    {
        if (m_imagesInFlightFences[imageIndex])
        {
            if (m_device.waitForFences(m_imagesInFlightFences[imageIndex], true, UINT64_MAX) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Could not wait for fences to complete.");
            }
        }
    }

    void Device::SyncImageAndFrameFence(uint32_t imageIndex)
    {
        m_imagesInFlightFences[imageIndex] = m_framesInFlightFences[m_currentFrameIndex];
    }

    void Device::ResetFrameFence()
    {
        m_device.resetFences(m_framesInFlightFences[m_currentFrameIndex]);
    }

    void Device::MapMemory(uint32_t bufferId, std::vector<vertex::Vertex> vertices, size_t size)
    {
        void* mappedData;
        auto allocation = m_buffers.at(bufferId).second;
        m_allocator.mapMemory(allocation, &mappedData);
        memcpy(mappedData, vertices.data(), size);
        m_allocator.unmapMemory(allocation);
    }

    void Device::MapMemory(uint32_t bufferId, std::vector<uint32_t> indices, size_t size)
    {
        void* mappedData;
        auto allocation = m_buffers.at(bufferId).second;
        m_allocator.mapMemory(allocation, &mappedData);
        memcpy(mappedData, indices.data(), size);
        m_allocator.unmapMemory(allocation);
    }

    QueueFamilyIndices::QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
    {
        uint32_t queueFamilyCount = 0;
        device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

        std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
        device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());
        
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            vk::Bool32 presentSupport = false;
            if (device.getSurfaceSupportKHR(i, surface, &presentSupport) != vk::Result::eSuccess)
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
    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
    {
        SwapChainSupportDetails details;
        if (device.getSurfaceCapabilitiesKHR(surface, &details.capabilities) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface capabilities.");
        }

        uint32_t formatCount;
        if (device.getSurfaceFormatsKHR(surface, &formatCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
        }

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            if (device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface formats.");
            }
        }

        uint32_t presentModeCount;
        if (device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
        }

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            if (device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("SwapChain::QuerySwapChainSupport() - Could not enumerate surface present modes.");
            }
        } 
        return details;
    }
}