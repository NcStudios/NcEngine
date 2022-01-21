#include "Swapchain.h"
#include "Initializers.h"
#include "Base.h"
#include "debug/NcError.h"

namespace nc::graphics
{
    Swapchain::Swapchain(Base* base, Vector2 dimensions)
        : m_base{ base },
          m_swapChain{},
          m_swapChainImages{},
          m_swapChainImageFormat{},
          m_swapChainExtent{},
          m_swapChainImageViews{},
          m_imagesInFlightFences{},
          m_framesInFlightFences{},
          m_imageAvailableSemaphores{},
          m_renderFinishedSemaphores{},
          m_currentFrameIndex{0}
    {
        Create(dimensions);
        CreateSynchronizationObjects();
    }
    
    Swapchain::~Swapchain() noexcept
    {
        Cleanup();
        DestroySynchronizationObjects();
    }

    void Swapchain::DestroySynchronizationObjects() noexcept
    {
        auto device = m_base->GetDevice();

        for (size_t i = 0; i < MaxFramesInFlight; ++i)
        {
            device.destroySemaphore(m_imageAvailableSemaphores[i]);
            device.destroySemaphore(m_renderFinishedSemaphores[i]);
            device.destroyFence(m_framesInFlightFences[i]);
        }
    }

    void Swapchain::Cleanup() noexcept
    {
        auto device = m_base->GetDevice();

        for (auto& imageView : m_swapChainImageViews)
        {
           device.destroyImageView(imageView, nullptr);
        }

       device.destroySwapchainKHR(m_swapChain);
    }

    // The semaphores deal solely with the GPU. Since rendering to an image taken from the swapchain and returning that image back to the swap chain are both asynchronous, 
    // the semaphores below tell the GPU when either step can begin for a single image. Vulkan will render multiple swapchain images very rapidly, so MaxFramesInFlight here creates 
    // a pair of semaphores for each frame up to MaxFramesInFlight. 
    // The fences synchronize GPU - CPU and they are what limit Vulkan to submitting only MaxFramesInFlight amount of frame-render jobs to the command queues. 
    // The fences in framesInFlightFences (one per frame in MAX_FRAMES_PER_FLIGHT) prevent more frame-render jobs than fences from being submitted until one frame-render job completes.
    // The fences in imagesInFlightFences (one per swapchain image) track for each swap chain image whether it is being used by a frame in flight.
    void Swapchain::CreateSynchronizationObjects()
    {
        m_imageAvailableSemaphores.resize(MaxFramesInFlight);
        m_renderFinishedSemaphores.resize(MaxFramesInFlight);
        m_framesInFlightFences.resize(MaxFramesInFlight);
        m_imagesInFlightFences.resize(m_swapChainImages.size(), nullptr); // To start, no frames in flight are using swapchain images, so explicitly initialize to nullptr.

        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        auto device =  m_base->GetDevice();
        for (size_t i = 0; i < MaxFramesInFlight; ++i)
        {
            m_imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
            m_renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
            m_framesInFlightFences[i] = device.createFence(fenceInfo);
        }
    }

    uint32_t Swapchain::GetFrameIndex() const noexcept
    {
        return m_currentFrameIndex;
    }

    void Swapchain::Present(uint32_t imageIndex, bool& isSwapChainValid)
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(&m_renderFinishedSemaphores[m_currentFrameIndex]); // Wait on this semaphore before presenting.

        vk::SwapchainKHR swapChains[] = {m_swapChain};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains); // Sets the swapchain(s) to present to
        presentInfo.setPImageIndices(&imageIndex); //  Sets the index of the image for each swapchain.
        presentInfo.setPResults(nullptr);

        auto result = m_base->GetQueue(QueueFamilyType::PresentFamily).presentKHR(&presentInfo);
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
    
    const std::vector<vk::Semaphore>& Swapchain::GetSemaphores(SemaphoreType semaphoreType) const noexcept
    {
        return semaphoreType == SemaphoreType::ImageAvailableForRender ? m_imageAvailableSemaphores : m_renderFinishedSemaphores;
    }

    void Swapchain::WaitForImageFence(uint32_t imageIndex)
    {
        if (m_imagesInFlightFences[imageIndex])
        {
            if (m_base->GetDevice().waitForFences(m_imagesInFlightFences[imageIndex], true, UINT64_MAX) != vk::Result::eSuccess)
            {
                throw NcError("Could not wait for fences to complete.");
            }
        }
    }

    void Swapchain::SyncImageAndFrameFence(uint32_t imageIndex)
    {
        m_imagesInFlightFences[imageIndex] = m_framesInFlightFences[m_currentFrameIndex];
    }

    const vk::Format& Swapchain::GetFormat() const noexcept
    {
        return m_swapChainImageFormat;
    }

    void Swapchain::Create(Vector2 dimensions)
    {
        auto swapChainSupport = m_base->QuerySwapChainSupport(m_base->GetPhysicalDevice(), m_base->GetSurface());

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
        createInfo.setSurface(m_base->GetSurface());
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        auto queueFamilies = QueueFamilyIndices(m_base->GetPhysicalDevice(), m_base->GetSurface());
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

        auto device = m_base->GetDevice();
        m_swapChain = device.createSwapchainKHR(createInfo);

        // Set swapchain images        
        uint32_t swapChainImageCount;
        if (device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Error getting swapchain images count.");
        }

        m_swapChainImages.resize(swapChainImageCount);
        if (device.getSwapchainImagesKHR(m_swapChain, &swapChainImageCount, m_swapChainImages.data()) != vk::Result::eSuccess)
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

            if (device.createImageView(&imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]) != vk::Result::eSuccess)
            {
                throw NcError("Failed to create image view");
            }
        }
    }

    void Swapchain::WaitForFrameFence() const
    {
        if (m_base->GetDevice().waitForFences(m_framesInFlightFences[m_currentFrameIndex], true, UINT64_MAX) != vk::Result::eSuccess)
        {
            throw NcError("Could not wait for fences to complete.");
        }
    }

    void Swapchain::IncrementFrameIndex()
    {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % MaxFramesInFlight;
    }

    void Swapchain::ResetFrameFence()
    {
        m_base->GetDevice().resetFences(m_framesInFlightFences[m_currentFrameIndex]);
    }

    const Vector2 Swapchain::GetExtentDimensions() const noexcept
    {
        return Vector2(m_swapChainExtent.width, m_swapChainExtent.height);
    }

    const vk::Extent2D& Swapchain::GetExtent() const noexcept
    {
        return m_swapChainExtent;
    }

    const std::vector<vk::ImageView>& Swapchain::GetColorImageViews() const noexcept
    {
        return m_swapChainImageViews;
    }

    uint32_t Swapchain::GetNextRenderReadyImageIndex(bool& isSwapChainValid)
    {
        auto resultAndIndex = m_base->GetDevice().acquireNextImageKHR(m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrameIndex]);
        if (resultAndIndex.result == vk::Result::eErrorOutOfDateKHR)
        {
            isSwapChainValid = false;
        }

        return resultAndIndex.value;
    }

    const std::vector<vk::Fence>& Swapchain::GetFences(FenceType fenceType) const noexcept
    {
        return fenceType == FenceType::FramesInFlight ? m_framesInFlightFences : m_imagesInFlightFences;
    }
}