#include "Swapchain.h"
#include "Initializers.h"
#include "PerFrameGpuContext.h"
#include "QueueFamily.h"
#include "core/Device.h"

#include "ncutility/NcError.h"

#include <algorithm>

#include <iostream>

namespace
{
auto DetermineFormat(std::span<const vk::SurfaceFormatKHR> availableFormats) -> vk::SurfaceFormatKHR
{
    NC_ASSERT(!availableFormats.empty(), "No surface formats found.");
    const auto pos = std::ranges::find_if(availableFormats, [](const auto& format)
    {
        return format.format == vk::Format::eB8G8R8A8Srgb &&
               format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });

    return pos != availableFormats.end() ? *pos : availableFormats[0];
}

auto DeterminePresentMode(std::span<const vk::PresentModeKHR> availablePresentModes) -> vk::PresentModeKHR
{
    NC_ASSERT(!availablePresentModes.empty(), "No present modes found.");
    const auto pos = std::ranges::find_if(availablePresentModes, [](const auto& presentMode)
    {
        return presentMode == vk::PresentModeKHR::eFifo;
    });

    return pos != availablePresentModes.end() ? *pos : vk::PresentModeKHR::eFifo;
}

auto DetermineExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const nc::Vector2& dimensions) -> vk::Extent2D
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    auto actualExtent = vk::Extent2D
    {
        static_cast<uint32_t>(dimensions.x),
        static_cast<uint32_t>(dimensions.y)
    };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
}

auto DetermineImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) -> uint32_t
{
    const auto imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        return capabilities.maxImageCount;
    }

    return imageCount;
}

auto GetSwapChainImages(vk::Device device, vk::SwapchainKHR swapChain) -> std::vector<vk::Image>
{
    auto imageCount = uint32_t{};
    if (device.getSwapchainImagesKHR(swapChain, &imageCount, nullptr) != vk::Result::eSuccess)
    {
        throw nc::NcError("Could not get swapchain images.");
    }

    auto images = std::vector<vk::Image>(imageCount);
    if (device.getSwapchainImagesKHR(swapChain, &imageCount, images.data()) != vk::Result::eSuccess)
    {
        throw nc::NcError("Could not get swapchain images.");
    }

    return images;
}

auto CreateSwapChainImageViews(const std::vector<vk::Image>& images, vk::Device device, vk::Format format) -> std::vector<vk::UniqueImageView>
{
    auto imageViews = std::vector<vk::UniqueImageView>{};
    imageViews.reserve(images.size());
    std::ranges::transform(images, std::back_inserter(imageViews), [device, &format](auto&& image)
    {
        return device.createImageViewUnique(vk::ImageViewCreateInfo
        {
            vk::ImageViewCreateFlags(), // ImageViewCreateFlags
            image,                      // Image
            vk::ImageViewType::e2D,     // ViewType
            format,                     // Format
            vk::ComponentMapping{},     // Components
            vk::ImageSubresourceRange // SubresourceRange
            {
                vk::ImageAspectFlagBits::eColor, // AspectMask
                0u, // BaseMipLevel
                1u, // LevelCount
                0u, // BaseArrayLayer
                1u  // LayerCount
            }
        });
    });

    return imageViews;
}
} // anonymous namespace

namespace nc::graphics::vulkan
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

    Swapchain::Swapchain(const Device& device, vk::SurfaceKHR surface, const Vector2& dimensions)
        : m_device{ device.VkDevice() },
          m_surface{surface},
          m_swapChainImageFormat{}
    {
        Create(device, surface, dimensions);
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

    auto Swapchain::PresentImageToSwapChain(PerFrameGpuContext* currentFrame, vk::Queue queue, uint32_t imageIndex) -> bool
    {
        std::cerr << "begin PresentImageToSwapChain\n";

        std::cerr << "RenderFinishedSemaphore\n";

        const auto waitSemaphore = currentFrame->RenderFinishedSemaphore();

        std::cerr << "m_swapChain: " << (bool)m_swapChain << '\n';

        vk::SwapchainKHR swapChains[] = {m_swapChain.get()};

        std::cerr << "create PresentInfoKHR\n";

        const auto presentInfo = vk::PresentInfoKHR
        (
            1u,             // WaitSemaphoreCount
            &waitSemaphore, // WaitSemaphore
            1u,             // Swapchain Count
            swapChains,     // PSwapchains
            &imageIndex,    // PImageIndices
            nullptr         // PResults
        );

        std::cerr << "set PresentKHR\n";

        const auto result = queue.presentKHR(&presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            std::cerr << "end PresentImageToSwapChain\n";

            return false;
        }

        if (result != vk::Result::eSuccess)
        {
            throw NcError("Could not present to the swapchain.");
        }

        std::cerr << "end PresentImageToSwapChain\n";

        return true;
    }

    void Swapchain::WaitImageReadyForBuffer(PerFrameGpuContext* currentFrame, uint32_t imageIndex)
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

    void Swapchain::Create(const Device& device, vk::SurfaceKHR surface, Vector2 dimensions)
    {
        const auto [capabilities, formats, presentModes] = QuerySwapChainSupport(device.VkPhysicalDevice(), surface);
        const auto surfaceFormat = ::DetermineFormat(formats);
        const auto presentMode = ::DeterminePresentMode(presentModes);
        const auto extent = ::DetermineExtent(capabilities, dimensions);
        const auto imageCount = ::DetermineImageCount(capabilities);
        const auto& queueFamilies = device.GetQueueIndices();
        const uint32_t queueFamilyIndices[] = {queueFamilies.GraphicsFamilyIndex(), queueFamilies.PresentFamilyIndex()};

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

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
        m_swapChainImages = ::GetSwapChainImages(m_device, m_swapChain.get());
        m_swapChainImageViews = ::CreateSwapChainImageViews(m_swapChainImages, m_device, surfaceFormat.format);
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
        currentFrame->WaitForSync(); // Wait until the command buffer has finished execution.
        auto [result, index] = m_device.acquireNextImageKHR(m_swapChain.get(), UINT64_MAX, currentFrame->ImageAvailableSemaphore());
        *imageIndex = index;
        return result != vk::Result::eErrorOutOfDateKHR;
    }

    void Swapchain::Resize(const Device& device, const Vector2& dimensions)
    {
        Cleanup();
        Create(device, m_surface, dimensions);
    }
} // namespace nc::graphics::vulkan
