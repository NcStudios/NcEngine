#include "Swapchain.h"
#include "graphics/Initializers.h"
#include "graphics/PerFrameGpuContext.h"
#include "graphics/QueueFamily.h"
#include "ncutility/NcError.h"

#include <algorithm>

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
          m_physicalDevice{physicalDevice},
          m_surface{surface},
          m_swapChainImageFormat{}
    {
        Create(physicalDevice, surface, dimensions);
        m_imagesInFlightFences.resize(m_swapChainImages.size(), nullptr);
    }
    
    Swapchain::~Swapchain() noexcept
    {
        Cleanup();
    }

    void Swapchain::Cleanup() noexcept
    {
        for (auto& imageView : m_swapChainImageViews)
        {
           imageView.reset();
        }
        m_swapChainImageViews.clear();
        m_swapChain.reset();
        m_imagesInFlightFences.resize(m_swapChainImages.size(), nullptr);
    }

    void Swapchain::Present(PerFrameGpuContext* currentFrame, vk::Queue queue, uint32_t imageIndex, bool& isSwapChainValid)
    {
        const auto waitSemaphore = currentFrame->RenderFinishedSemaphore();
        vk::SwapchainKHR swapChains[] = {m_swapChain.get()};

        const auto presentInfo = vk::PresentInfoKHR
        (
            1u,             // WaitSemaphoreCount
            &waitSemaphore, // WaitSemaphore
            1u,             // Swapchain Count
            swapChains,     // PSwapchains
            &imageIndex,    // PImageIndices
            nullptr         // PResults
        );

        const auto result = queue.presentKHR(&presentInfo);
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
    
    void Swapchain::WaitForNextImage(PerFrameGpuContext* currentFrame, uint32_t imageIndex)
    {
        if (m_imagesInFlightFences[imageIndex])
        {
            if (m_device.waitForFences(m_imagesInFlightFences[imageIndex], true, UINT64_MAX) != vk::Result::eSuccess)
            {
                throw NcError("Could not wait for fences to complete.");
            }
        }

        m_imagesInFlightFences[imageIndex] = currentFrame->Fence();
    }

    const vk::Format& Swapchain::GetFormat() const noexcept
    {
        return m_swapChainImageFormat;
    }

    void Swapchain::Create(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, Vector2 dimensions)
    {
        auto [capabilities, formats, presentModes] = QuerySwapChainSupport(physicalDevice, surface);

        auto surfaceFormat = formats[0];
        const auto surfaceFormatPos = std::ranges::find_if(formats, [](const auto &availableFormat)
        {
            return (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
        });
        if (surfaceFormatPos != formats.end()) surfaceFormat = *surfaceFormatPos;

        auto presentMode = vk::PresentModeKHR::eFifo;
        const auto presentModePos = std::ranges::find_if(presentModes, [](const auto &availablePresentMode)
        {
            return availablePresentMode == vk::PresentModeKHR::eMailbox;
        });
        if (presentModePos != presentModes.end()) presentMode = *presentModePos;

        vk::Extent2D extent;
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = capabilities.currentExtent;
        }
        else
        {
            vk::Extent2D actualExtent = 
            {
                static_cast<uint32_t>(dimensions.x),
                static_cast<uint32_t>(dimensions.y)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
            extent = actualExtent;
        }

        auto imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        const auto queueFamilies = QueueFamilyIndices(physicalDevice, surface);
        const uint32_t queueFamilyIndices[] = {queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily)};

        m_swapChain = m_device.createSwapchainKHRUnique(
        vk::SwapchainCreateInfoKHR
        {
            vk::SwapchainCreateFlagsKHR(), // SwapchainCreateFlagsKHR
            surface,                       // Surface
            imageCount,                    // MinImageCount
            surfaceFormat.format,          // ImageFormat
            surfaceFormat.colorSpace,      // ImageColorSpace
            extent,                        // ImageExtent
            1u,                            // ImageArrayLayers
            vk::ImageUsageFlagBits::eColorAttachment,         // ImageUsage
            queueFamilies.IsSeparatePresentQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, // ImageSharingMode
            queueFamilies.IsSeparatePresentQueue() ? 2u : 0u,                                                    // QueueFamilyIndexCount
            queueFamilies.IsSeparatePresentQueue() ? queueFamilyIndices : nullptr,                               // PQueueFamilyIndices
            capabilities.currentTransform,          // PreTransform
            vk::CompositeAlphaFlagBitsKHR::eOpaque, // CompositeAlpha
            presentMode,                            // PresentMode
            VK_TRUE                                 // Clipped
        });

        // Set swapchain images        
        uint32_t swapChainImageCount;
        if (m_device.getSwapchainImagesKHR(m_swapChain.get(), &swapChainImageCount, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Error getting swapchain images count.");
        }

        m_swapChainImages.resize(swapChainImageCount);
        if (m_device.getSwapchainImagesKHR(m_swapChain.get(), &swapChainImageCount, m_swapChainImages.data()) != vk::Result::eSuccess)
        {
            throw NcError("Error getting swapchain images.");
        }

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
        m_swapChainImageViews.reserve(m_swapChainImages.size());

        for (auto &swapChainImage : m_swapChainImages)
        {
            m_swapChainImageViews.emplace_back(m_device.createImageViewUnique(
            {
                vk::ImageViewCreateFlags(), // ImageViewCreateFlags
                swapChainImage,             // Image
                vk::ImageViewType::e2D,     // ViewType
                m_swapChainImageFormat,     // Format
                vk::ComponentMapping        // Components
                {
                    vk::ComponentSwizzle::eIdentity, // R
                    vk::ComponentSwizzle::eIdentity, // G
                    vk::ComponentSwizzle::eIdentity, // B
                    vk::ComponentSwizzle::eIdentity  // A
                },
                vk::ImageSubresourceRange // SubresourceRange
                {
                    vk::ImageAspectFlagBits::eColor, // AspectMask
                    0u, // BaseMipLevel
                    1u, // LevelCount
                    0u, // BaseArrayLayer
                    1u  // LayerCount
                }
            }));
        }
    }

    const vk::Extent2D& Swapchain::GetExtent() const noexcept
    {
        return m_swapChainExtent;
    }

    const std::vector<vk::UniqueImageView>& Swapchain::GetColorImageViews() const noexcept
    {
        return m_swapChainImageViews;
    }

    bool Swapchain::GetNextRenderReadyImageIndex(PerFrameGpuContext* currentFrame, uint32_t* imageIndex)
    {
        auto [result, index] = m_device.acquireNextImageKHR(m_swapChain.get(), UINT64_MAX, currentFrame->ImageAvailableSemaphore());
        *imageIndex = index;
        return result != vk::Result::eErrorOutOfDateKHR;
    }

    void Swapchain::Resize(const Vector2& dimensions)
    {
        Cleanup();
        Create(m_physicalDevice, m_surface, dimensions);
    }
}