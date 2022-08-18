#include "Base.h"
#include "config/Config.h"
#include "debug/NcError.h"
#include "graphics/Commands.h"
#include "stb/stb_image.h"
#include "vk/Initializers.h"
#include "vk/Swapchain.h"

#include <algorithm>
#include <array>
#include <set>
#include <string>

namespace
{
    const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const std::vector<const char*> GlobalExtensions = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
    const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

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
} // anonymous namespace

namespace nc::graphics
{
    Base::Base(HWND hwnd, HINSTANCE hinstance)
    : m_instance{},
        m_surface{},
        m_logicalDevice{},
        m_physicalDevice{},
        m_graphicsQueue{},
        m_presentQueue{},
        m_depthFormat{},
        m_samplesCount{},
        m_samplesInitialized{false},
        m_commandPool{},
        m_imguiDescriptorPool{}
    {
        CreateInstance();
        CreateSurface(hwnd, hinstance);
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateCommandQueues();
        CreateCommandPool();
        QueryDepthFormatSupport();
        CreateDescriptorPools();
    }

    Base::~Base() noexcept
    {
        m_logicalDevice.destroyDescriptorPool(m_imguiDescriptorPool);
        m_logicalDevice.destroyCommandPool(m_commandPool);
        m_instance.destroySurfaceKHR(m_surface);
        m_logicalDevice.destroy();
        m_instance.destroy();
    }

    void Base::FreeCommandBuffers(std::vector<vk::CommandBuffer>* commandBuffers) noexcept
    {
        m_logicalDevice.freeCommandBuffers(m_commandPool, static_cast<uint32_t>(commandBuffers->size()), commandBuffers->data());
    }

    void Base::CreateInstance()
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        SetValidationLayersEnabled(nc::config::GetGraphicsSettings().useValidationLayers, instanceCreateInfo);
        SetGlobalExtensions(instanceCreateInfo);

        try
        {
            m_instance = createInstance(instanceCreateInfo);
        }
        catch (const std::exception&)
        {
            std::throw_with_nested(NcError("Failed to create instance."));
        }
    }

    void Base::CreateSurface(HWND hwnd, HINSTANCE hinstance)
    {
        vk::SurfaceKHR surface;
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.setHinstance(hinstance);
        surfaceCreateInfo.setHwnd(hwnd);
        if (m_instance.createWin32SurfaceKHR(&surfaceCreateInfo, nullptr, &m_surface) != vk::Result::eSuccess)
        {
            throw NcError("Failed to get surface.");
        }
    }

    void Base::CreatePhysicalDevice()
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
            throw NcError("Test physical devices for suitability - Failed to find GPU that supports Vulkan.");
        }
    }

    void Base::CreateCommandQueues()
    {
        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        m_graphicsQueue = m_logicalDevice.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), 0);
        m_presentQueue = m_logicalDevice.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily), 0);
    }

    void Base::CreateDescriptorPools()
    {
        // Create descriptor pool for IMGUI
        std::array<vk::DescriptorPoolSize, 11> imguiPoolSizes =
        {
            vk::DescriptorPoolSize { vk::DescriptorType::eSampler, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageImage, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eInputAttachment, 1000 }
        };

        vk::DescriptorPoolCreateInfo imguiDescriptorPoolInfo = {};
        imguiDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        imguiDescriptorPoolInfo.setMaxSets(1000);
        imguiDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(imguiPoolSizes.size()));
        imguiDescriptorPoolInfo.setPPoolSizes(imguiPoolSizes.data());

        if (m_logicalDevice.createDescriptorPool(&imguiDescriptorPoolInfo, nullptr, &m_imguiDescriptorPool) != vk::Result::eSuccess)
        {
            throw NcError("Could not create ImGUI descriptor pool.");
        }
    }

    void Base::InitializeImgui(const vk::RenderPass& defaultPass)
    {
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = m_instance;
        initInfo.PhysicalDevice = m_physicalDevice;
        initInfo.Device = m_logicalDevice;
        initInfo.Queue = m_graphicsQueue;
        initInfo.DescriptorPool = m_imguiDescriptorPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.MSAASamples = VkSampleCountFlagBits(GetMaxSamplesCount());

        ImGui_ImplVulkan_Init(&initInfo, defaultPass);

        Commands::SubmitCommandImmediate(*this, [&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    vk::SampleCountFlagBits Base::GetMaxSamplesCount()
    {
        if (m_samplesInitialized == true)
        {
            return m_samplesCount;
        }

        m_samplesInitialized = true;

        vk::PhysicalDeviceProperties properties{};
        m_physicalDevice.getProperties(&properties);

        auto antialiasingSamples = nc::config::GetGraphicsSettings().antialiasing;
        vk::SampleCountFlags countsFromConfig = vk::SampleCountFlagBits::e1;

        if      (antialiasingSamples >= 64) countsFromConfig = vk::SampleCountFlagBits::e64;
        else if (antialiasingSamples >= 32) countsFromConfig = vk::SampleCountFlagBits::e32;
        else if (antialiasingSamples >= 16) countsFromConfig = vk::SampleCountFlagBits::e16;
        else if (antialiasingSamples >= 8)  countsFromConfig = vk::SampleCountFlagBits::e8;
        else if (antialiasingSamples >= 4)  countsFromConfig = vk::SampleCountFlagBits::e4;
        else if (antialiasingSamples >= 2)  countsFromConfig = vk::SampleCountFlagBits::e2;
        else countsFromConfig = vk::SampleCountFlagBits::e1;

        auto counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
        if (countsFromConfig < counts)
        {
            counts = countsFromConfig;
        }

        if      (counts & vk::SampleCountFlagBits::e64) m_samplesCount = vk::SampleCountFlagBits::e64;
        else if (counts & vk::SampleCountFlagBits::e32) m_samplesCount = vk::SampleCountFlagBits::e32;
        else if (counts & vk::SampleCountFlagBits::e16) m_samplesCount = vk::SampleCountFlagBits::e16;
        else if (counts & vk::SampleCountFlagBits::e8)  m_samplesCount = vk::SampleCountFlagBits::e8;
        else if (counts & vk::SampleCountFlagBits::e4)  m_samplesCount = vk::SampleCountFlagBits::e4;
        else if (counts & vk::SampleCountFlagBits::e2)  m_samplesCount = vk::SampleCountFlagBits::e2;
        else m_samplesCount = vk::SampleCountFlagBits::e1;

        return m_samplesCount;
    }

    void Base::CreateLogicalDevice()
    {
        auto indices = QueueFamilyIndices(m_physicalDevice, m_surface);

        std::set<uint32_t> uniqueQueueFamilies = {};

        uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily));

        if (indices.IsSeparatePresentQueue())
        {
            uniqueQueueFamilies.emplace(indices.GetQueueFamilyIndex(QueueFamilyType::PresentFamily));
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

        try
        {
            m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);
        }
        catch (const std::exception&)
        {
            std::throw_with_nested(NcError("Failed to create device."));
        }

        m_gpuProperties = m_physicalDevice.getProperties();
    }

    void Base::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        poolInfo.setQueueFamilyIndex(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily));
        m_commandPool = m_logicalDevice.createCommandPool(poolInfo);
    }

    void Base::TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, uint32_t layerCount, vk::ImageLayout newLayout)
    {
        Commands::SubmitCommandImmediate(*this, [&](vk::CommandBuffer cmd) 
        { 
            vk::ImageMemoryBarrier barrier{};
            barrier.setOldLayout(oldLayout);
            barrier.setNewLayout(newLayout);
            barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.setImage(image);

            vk::ImageSubresourceRange subresourceRange{};
            subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            subresourceRange.setBaseMipLevel(0);
            subresourceRange.setLevelCount(1);
            subresourceRange.setBaseArrayLayer(0);
            subresourceRange.setLayerCount(layerCount);

            barrier.setSubresourceRange(subresourceRange);

            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;

            if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                barrier.setSrcAccessMask(vk::AccessFlags());
                barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }
            else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
                sourceStage = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else 
            {
                throw NcError("Unsupported layout transition.");
            }

            cmd.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }

    void Base::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
    {
        Commands::SubmitCommandImmediate(*this, [&](vk::CommandBuffer cmd) 
        {
            vk::BufferImageCopy region{};
            region.setBufferOffset(0);
            region.setBufferRowLength(0);
            region.setBufferImageHeight(0);

            vk::ImageSubresourceLayers subresource{};
            subresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
            subresource.setMipLevel(0);
            subresource.setBaseArrayLayer(0);
            subresource.setLayerCount(1);

            region.setImageSubresource(subresource);
            region.setImageOffset({0, 0, 0});
            region.setImageExtent({width, height, 1});

            cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
        });
    }

    void Base::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        Commands::SubmitCommandImmediate(*this, [&](vk::CommandBuffer cmd) 
        {
            vk::BufferImageCopy region{};
            region.setBufferOffset(0);
            region.setBufferRowLength(0);
            region.setBufferImageHeight(0);

            vk::ImageSubresourceLayers subresource{};
            subresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
            subresource.setMipLevel(0);
            subresource.setBaseArrayLayer(0);
            subresource.setLayerCount(layerCount);

            region.setImageSubresource(subresource);
            region.setImageOffset({0, 0, 0});
            region.setImageExtent({width, height, 1});

            cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
        });
    }

    const vk::Device& Base::GetDevice() const noexcept
    {
        return m_logicalDevice;
    }

    const vk::PhysicalDevice& Base::GetPhysicalDevice() const noexcept
    {
        return m_physicalDevice;
    }

    const vk::Instance& Base::GetInstance() const noexcept
    {
        return m_instance;
    }

    const vk::SurfaceKHR& Base::GetSurface() const noexcept
    {
        return m_surface;
    }

    const vk::CommandPool& Base::GetCommandPool() const noexcept
    {
        return m_commandPool;
    }

    const vk::Queue& Base::GetQueue(QueueFamilyType type) const noexcept
    {
        return type == QueueFamilyType::GraphicsFamily ? m_graphicsQueue : m_presentQueue;
    }

    const vk::Format& Base::GetDepthFormat() const noexcept
    {
        return m_depthFormat;
    }

    void Base::QueryDepthFormatSupport()
    {
        std::vector<vk::Format> depthFormats = { vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };

        for (auto& format : depthFormats)
        {
            vk::FormatProperties formatProperties;
            m_physicalDevice.getFormatProperties(format, &formatProperties);
            // Format must support depth stencil attachment for optimal tiling
            if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            {
                m_depthFormat = format;
                return;
            }
        }
        throw NcError("Could not find a matching depth format");
    }
}  // namespace nc::graphics
