#include "Base.h"
#include "graphics/Mesh.h"
#include "graphics/Commands.h"
#include "stb/stb_image.h"

#include <set>
#include <string>
#include <algorithm>

namespace
{
    const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const std::vector<const char*> GlobalExtensions = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
    const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    #ifdef NC_USE_VALIDATION
        const bool IsValidationLayersEnabled = true;
    #else
        const bool IsValidationLayersEnabled = false;
    #endif

    bool CheckValidationLayerSupport();
    void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo);
    void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo);

    void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo)
    {
         if (IsValidationLayersEnabled)
        {
            if (!CheckValidationLayerSupport())
            {
                throw std::runtime_error("Instance::EnableValidationLayers - Validation layers requested but not available.");
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
}

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
      m_bufferIndex{0},
      m_buffers{},
      m_imageIndex{0},
      m_images{},
      m_allocator{},
      m_commandPool{},
      m_imguiDescriptorPool{},
      m_renderingDescriptorPool{}
    {
        CreateInstance();
        CreateSurface(hwnd, hinstance);
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateCommandQueues();
        CreateCommandPool();
        CreateAllocator();
        QueryDepthFormatSupport();
        CreateDescriptorPools();
    }

    Base::~Base()
    {
        m_logicalDevice.destroyDescriptorPool(m_renderingDescriptorPool);
        m_logicalDevice.destroyDescriptorPool(m_imguiDescriptorPool);
        m_logicalDevice.destroyCommandPool(m_commandPool);

        for (uint32_t i = 0; i < m_buffers.size(); ++i)
        {
            m_allocator.destroyBuffer(m_buffers[i].first, m_buffers[i].second);
        }
        
        for (uint32_t i = 0; i < m_images.size(); ++i)
        {
            m_allocator.destroyImage(m_images[i].first, m_images[i].second);
        }

        m_instance.destroySurfaceKHR(m_surface);
        m_allocator.destroy();
        m_logicalDevice.destroy();
        m_instance.destroy();
    }

    void Base::FreeCommandBuffers(std::vector<vk::CommandBuffer>* commandBuffers)
    {
        m_logicalDevice.freeCommandBuffers(m_commandPool, static_cast<uint32_t>(commandBuffers->size()), commandBuffers->data());
    }

    void Base::CreateInstance()
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        EnableValidationLayers(instanceCreateInfo);
        SetGlobalExtensions(instanceCreateInfo);

        try
        {
            m_instance = createInstance(instanceCreateInfo);
        }
        catch (const std::exception& error)
        {
            std::throw_with_nested(std::runtime_error("Failed to create instance."));
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
            throw std::runtime_error("Failed to get surface.");
        }
    }

    vk::DescriptorPool* Base::GetRenderingDescriptorPoolPtr() noexcept
    {
        return &m_renderingDescriptorPool;
    }

    void Base::CreatePhysicalDevice()
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
                throw std::runtime_error("Base::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
            }

            std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
            if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != vk::Result::eSuccess)
            {
                throw std::runtime_error("Base::IsPhysicalDeviceExtensionSupported() - could not enumerate device extensions.");
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
            throw std::runtime_error("Test physical devices for suitability - Failed to find GPU that supports Vulkan.");
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
        imguiDescriptorPoolInfo.setPoolSizeCount(imguiPoolSizes.size());
        imguiDescriptorPoolInfo.setPPoolSizes(imguiPoolSizes.data());

        if (m_logicalDevice.createDescriptorPool(&imguiDescriptorPoolInfo, nullptr, &m_imguiDescriptorPool) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Could not create ImGUI descriptor pool.");
        }

        std::array<vk::DescriptorPoolSize, 3> renderingPoolSizes =
        {
            vk::DescriptorPoolSize { vk::DescriptorType::eSampler, 10 },
            vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 10 }
        };
        
        vk::DescriptorPoolCreateInfo renderingDescriptorPoolInfo = {};
        renderingDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        renderingDescriptorPoolInfo.setMaxSets(1000);
        renderingDescriptorPoolInfo.setPoolSizeCount(renderingPoolSizes.size());
        renderingDescriptorPoolInfo.setPPoolSizes(renderingPoolSizes.data());
        
        if (m_logicalDevice.createDescriptorPool(&renderingDescriptorPoolInfo, nullptr, &m_renderingDescriptorPool) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Could not create rendering descriptor pool.");
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

        ImGui_ImplVulkan_Init(&initInfo, defaultPass);

        Commands::SubmitCommandImmediate(*this, [&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    vma::Allocator* Base::GetAllocator() noexcept
    {
        return &m_allocator;
    }

    vma::Allocation* Base::GetBufferAllocation(uint32_t index)
    {
        return &(m_buffers.at(index)).second;
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

        vk::PhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.setSamplerAnisotropy(VK_TRUE);
        deviceFeatures.setFillModeNonSolid(VK_TRUE);
        deviceFeatures.setWideLines(VK_TRUE);

        vk::PhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
        indexingFeatures.setPNext(nullptr);
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
        catch (const std::exception& error)
        {
            std::throw_with_nested(std::runtime_error("Failed to create device."));
        }
    }

    void Base::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        poolInfo.setQueueFamilyIndex(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily));
        m_commandPool = m_logicalDevice.createCommandPool(poolInfo);
    }

    uint32_t Base::CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vma::MemoryUsage memoryUsageType, vk::Buffer* createdBuffer)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(size);
        bufferInfo.setUsage(usageFlags);

        vma::AllocationCreateInfo allocationInfo;
        allocationInfo.usage = memoryUsageType;

        vma::Allocation allocation;
        vk::Buffer buffer;
        auto result = m_allocator.createBuffer(&bufferInfo, &allocationInfo, &buffer, &allocation, nullptr);
        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error creating buffer.");
        }

        m_buffers.emplace(m_bufferIndex, std::pair{buffer, allocation});
        *createdBuffer = buffer;
        return m_bufferIndex++;
    }

    uint32_t Base::CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::Image* createdImage)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(format);
        imageInfo.setExtent( { static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y), 1 });
        imageInfo.setMipLevels(1);
        imageInfo.setArrayLayers(1);
        imageInfo.setSamples(vk::SampleCountFlagBits::e1);
        imageInfo.setTiling(vk::ImageTiling::eOptimal);
        imageInfo.setUsage(usageFlags);

        vma::AllocationCreateInfo allocationInfo;
        allocationInfo.usage = vma::MemoryUsage::eGpuOnly;

        vma::Allocation allocation;        
        vk::Image image;
        if (m_allocator.createImage(&imageInfo, &allocationInfo, &image, &allocation, nullptr) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Error creating image.");
        }

        m_images.emplace(m_imageIndex, std::pair{image, allocation});
        *createdImage = image;
        return m_imageIndex++;
    }

    uint32_t Base::CreateTexture(stbi_uc* pixels, uint32_t width, uint32_t height, vk::Image* createdImage)
    {
        vk::DeviceSize imageSize = width * height * 4;

        // Create staging buffer (lives on CPU)
        vk::Buffer stagingBuffer;
        auto stagingIndex = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, &stagingBuffer);

        // Map the pixels onto the staging buffer
        void* mappedData;
        auto allocation = m_buffers.at(stagingIndex).second;
        m_allocator.mapMemory(allocation, &mappedData);
        memcpy(mappedData, pixels, static_cast<size_t>(imageSize));
        m_allocator.unmapMemory(allocation);
        stbi_image_free(pixels);

        vk::Image textureImage;
        auto imageIndex = CreateImage(vk::Format::eR8G8B8A8Srgb, Vector2{static_cast<float>(width), static_cast<float>(height)}, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, &textureImage);

        TransitionImageLayout(textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        CopyBufferToImage(stagingBuffer, textureImage, width, height);
        TransitionImageLayout(textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        DestroyBuffer(stagingIndex);

        *createdImage = textureImage;
        return imageIndex;
    }

    vk::UniqueSampler Base::CreateTextureSampler()
    {
        vk::PhysicalDeviceProperties properties{};
        m_physicalDevice.getProperties(&properties);

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.setMagFilter(vk::Filter::eLinear);
        samplerInfo.setMinFilter(vk::Filter::eLinear);
        samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
        samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
        samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
        samplerInfo.setAnisotropyEnable(VK_TRUE);
        samplerInfo.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy);
        samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
        samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
        samplerInfo.setCompareEnable(VK_FALSE);
        samplerInfo.setCompareOp(vk::CompareOp::eAlways);
        samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
        samplerInfo.setMipLodBias(0.0f);
        samplerInfo.setMinLod(0.0f);
        samplerInfo.setMaxLod(0.0f);

        return m_logicalDevice.createSamplerUnique(samplerInfo);
    }
    
    void Base::TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
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
            subresourceRange.setLayerCount(1);

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
                throw std::runtime_error("Unsupported layout transition.");
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

    vk::UniqueImageView Base::CreateTextureView(const vk::Image& image)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(image);
        viewInfo.setViewType(vk::ImageViewType::e2D);
        viewInfo.setFormat(vk::Format::eR8G8B8A8Srgb);

        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        subresourceRange.setBaseMipLevel(0);
        subresourceRange.setLevelCount(1);
        subresourceRange.setBaseArrayLayer(0);
        subresourceRange.setLayerCount(1);

        viewInfo.setSubresourceRange(subresourceRange);
        return m_logicalDevice.createImageViewUnique(viewInfo);
    }

    void Base::DestroyBuffer(uint32_t id)
    {
        auto buffer = m_buffers.find(id);
        if (buffer == m_buffers.end())
        {
            throw std::runtime_error("The given buffer ID was not present in the dictionary.");
        }

        m_allocator.destroyBuffer(buffer->second.first, buffer->second.second);
        m_buffers.erase(id);
    }

    void Base::DestroyImage(uint32_t id)
    {
        auto image = m_images.find(id);
        if (image == m_images.end())
        {
            throw std::runtime_error("The given image ID was not present in the dictionary.");
        }

        m_allocator.destroyImage(image->second.first, image->second.second);
        m_images.erase(id);
    }

    void Base::CreateAllocator()
    {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorInfo.physicalDevice = m_physicalDevice;
        allocatorInfo.device = m_logicalDevice;
        allocatorInfo.instance = m_instance;
        m_allocator = vma::createAllocator(allocatorInfo);
    }

    const vk::Device& Base::GetDevice() const noexcept
    {
        return m_logicalDevice;
    }

    const vk::PhysicalDevice& Base::GetPhysicalDevice() const noexcept
    {
        return m_physicalDevice;
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

    void Base::MapMemory(uint32_t bufferId, std::vector<Vertex> vertices, size_t size)
    {
        void* mappedData;
        auto allocation = m_buffers.at(bufferId).second;
        m_allocator.mapMemory(allocation, &mappedData);
        memcpy(mappedData, vertices.data(), size);
        m_allocator.unmapMemory(allocation);
    }

    void Base::MapMemory(uint32_t bufferId, std::vector<uint32_t> indices, size_t size)
    {
        void* mappedData;
        auto allocation = m_buffers.at(bufferId).second;
        m_allocator.mapMemory(allocation, &mappedData);
        memcpy(mappedData, indices.data(), size);
        m_allocator.unmapMemory(allocation);
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
		throw std::runtime_error("Could not find a matching depth format");
    }

    const SwapChainSupportDetails Base::QuerySwapChainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const
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