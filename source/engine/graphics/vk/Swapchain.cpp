#include "Swapchain.h"
#include "Initializers.h"
#include "debug/NcError.h"
#include "QueueFamily.h"

namespace nc::graphics
{
    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
    {
        SwapChainSupportDetails details;
        if (device.getSurfaceCapabilitiesKHR(surface, &details.capabilities) != vk::Result::eSuccess)
        {
            throw NcError("Could not enumerate surface capabilities.");
        }

        uint32_t formatCount;
        if (device.getSurfaceFormatsKHR(surface, &formatCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Could not enumerate surface formats.");
        }

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            if (device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data()) != vk::Result::eSuccess)
            {
                throw NcError("Could not enumerate surface formats.");
            }
        }

        uint32_t presentModeCount;
        if (device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Could not enumerate surface present modes.");
        }

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            if (device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data()) != vk::Result::eSuccess)
            {
                throw NcError("Could not enumerate surface present modes.");
            }
        } 
        return details;
    }

    Swapchain::Swapchain(vk::Device device, vk::PhysicalDevice physicalDevice,
                         vk::SurfaceKHR surface, const Vector2& dimensions)
        : m_device{ device },
          m_swapChain{},
          m_swapChainImages{},
          m_swapChainImageFormat{},
          m_swapChainExtent{},
          m_swapChainImageViews{},
          m_perFrameGpuContext{},
          m_currentFrameIndex{0}
    {
        Create(physicalDevice, surface, dimensions);
        m_perFrameGpuContext.reserve(MaxFramesInFlight);
        for (auto i = 0; i < MaxFramesInFlight; i++)
        {
            m_perFrameGpuContext.emplace_back(m_device);
        }

        // The fences in imagesInFlightFences (one per swapchain image) track for each swap chain image whether it is being used by a frame in flight.
        m_imagesInFlightFences.resize(m_swapChainImages.size(), nullptr); // To start, no frames in flight are using swapchain images, so explicitly initialize to nullptr.
    }
    
    Swapchain::~Swapchain() noexcept
    {
        Cleanup();
    }

    void Swapchain::Cleanup() noexcept
    {
        for (auto& imageView : m_swapChainImageViews)
        {
           m_device.destroyImageView(imageView, nullptr);
        }

       m_device.destroySwapchainKHR(m_swapChain);
    }

    uint32_t Swapchain::GetFrameIndex() const noexcept
    {
        return m_currentFrameIndex;
    }

    void Swapchain::Present(vk::Queue queue, uint32_t imageIndex, bool& isSwapChainValid)
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(&(GetPerFrameGpuContext().RenderFinishedSemaphore())); // Wait on this semaphore before presenting.

        vk::SwapchainKHR swapChains[] = {m_swapChain};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains); // Sets the swapchain(s) to present to
        presentInfo.setPImageIndices(&imageIndex); //  Sets the index of the image for each swapchain.
        presentInfo.setPResults(nullptr);

        auto result = queue.presentKHR(&presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            isSwapChainValid = false;
            return;
        }
        
        if (result != vk::Result::eSuccess)
        {
            throw NcError("Could not present to the swapchain.");
        }
    }
    
    void Swapchain::WaitForImageFence(uint32_t imageIndex)
    {
        if (m_imagesInFlightFences[imageIndex])
        {
            if (m_device.waitForFences(m_imagesInFlightFences[imageIndex], true, UINT64_MAX) != vk::Result::eSuccess)
            {
                throw NcError("Could not wait for fences to complete.");
            }
        }
    }

    void Swapchain::SyncImageAndFrameFence(uint32_t imageIndex)
    {
        m_imagesInFlightFences[imageIndex] = GetPerFrameGpuContext().Fence();
    }

    const vk::Format& Swapchain::GetFormat() const noexcept
    {
        return m_swapChainImageFormat;
    }

    void Swapchain::Create(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, Vector2 dimensions)
    {
        auto swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

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
        createInfo.setSurface(surface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        auto queueFamilies = QueueFamilyIndices(physicalDevice, surface);
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
            throw NcError("Error getting swapchain images count.");
        }

        m_swapChainImages.resize(swapChainImageCount);
        if (m_device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, m_swapChainImages.data()) != vk::Result::eSuccess)
        {
            throw NcError("Error getting swapchain images.");
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
                throw NcError("Failed to create image view");
            }
        }
    }

    void Swapchain::WaitForFrameFence()
    {
        GetPerFrameGpuContext().Wait();
    }

    void Swapchain::IncrementFrameIndex()
    {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % MaxFramesInFlight;
    }

    void Swapchain::ResetFrameFence()
    {
        GetPerFrameGpuContext().Reset();
    }

    const Vector2 Swapchain::GetExtentDimensions() const noexcept
    {
        return Vector2(static_cast<float>(m_swapChainExtent.width), static_cast<float>(m_swapChainExtent.height));
    }

    const vk::Extent2D& Swapchain::GetExtent() const noexcept
    {
        return m_swapChainExtent;
    }

    const std::vector<vk::ImageView>& Swapchain::GetColorImageViews() const noexcept
    {
        return m_swapChainImageViews;
    }

    bool Swapchain::GetNextRenderReadyImageIndex(uint32_t* imageIndex)
    {
        auto [result, index] = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, m_perFrameGpuContext[m_currentFrameIndex].ImageAvailableSemaphore());
        *imageIndex = index;
        return result != vk::Result::eErrorOutOfDateKHR;
    }
}