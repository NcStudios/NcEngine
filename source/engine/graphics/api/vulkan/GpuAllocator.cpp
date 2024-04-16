#include "GpuAllocator.h"
#include "core/Device.h"
#include "core/Instance.h"

#include "ncutility/NcError.h"

namespace
{
    auto CreateAllocator(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::Instance instance) -> vma::Allocator
    {
        vma::AllocatorCreateInfo allocatorInfo{};
        vma::VulkanFunctions vulkanFunctions{};
        vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        allocatorInfo.pVulkanFunctions = &vulkanFunctions;
        allocatorInfo.physicalDevice = physicalDevice;
        allocatorInfo.device = logicalDevice;
        allocatorInfo.instance = instance;
        return vma::createAllocator(allocatorInfo);
    }
} // anonymous namespace

namespace nc::graphics::vulkan
{
    GpuAllocator::GpuAllocator(const Device* device, const Instance& instance)
        : m_device{device},
          m_allocator{::CreateAllocator(device->VkDevice(), device->VkPhysicalDevice(), instance.VkInstance())},
          m_deviceProperties{device->VkPhysicalDevice().getProperties()}
    {
    }

    GpuAllocator::~GpuAllocator() noexcept
    {
        m_allocator.destroy();
    }

    auto GpuAllocator::PadBufferOffsetAlignment(uint32_t originalSize, vk::DescriptorType bufferType) -> uint32_t
    {
        uint32_t minimumAlignment = 0;

        switch (bufferType)
        {
            case vk::DescriptorType::eStorageBuffer:
            {
                minimumAlignment = static_cast<uint32_t>(m_deviceProperties.limits.minStorageBufferOffsetAlignment);
                break;
            }
            case vk::DescriptorType::eUniformBuffer:
            {
                minimumAlignment = static_cast<uint32_t>(m_deviceProperties.limits.minUniformBufferOffsetAlignment);
                break;
            }
            default:
            {
                throw NcError("Invalid bufferType chosen.");
            }
        }

        uint32_t alignedSize = originalSize;

        if (minimumAlignment > 0)
        {
            alignedSize = (alignedSize + minimumAlignment - 1) & ~(minimumAlignment - 1);
        }

        return alignedSize;
    }

    void GpuAllocator::CopyBuffer(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size)
    {
        m_device->ExecuteCommand([sourceBuffer, destinationBuffer, size](vk::CommandBuffer cmd)
        {
            vk::BufferCopy copyRegion{};
            copyRegion.setSize(size);
            cmd.copyBuffer(sourceBuffer, destinationBuffer, 1, &copyRegion);
        });
    }

    auto GpuAllocator::CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vma::MemoryUsage usageType) -> GpuAllocation<vk::Buffer>
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.usage = usageFlags;

        vma::AllocationCreateInfo allocationInfo{};
        allocationInfo.usage = usageType;

        vk::Buffer buffer;
        vma::Allocation allocation;
        auto result = m_allocator.createBuffer(&bufferInfo, &allocationInfo, &buffer, &allocation, nullptr);
        if (result != vk::Result::eSuccess)
        {
            throw NcError("Error creating buffer.");
        }

        return GpuAllocation<vk::Buffer>{buffer, allocation, this};
    }

    auto GpuAllocator::CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::ImageCreateFlags imageFlags, uint32_t arrayLayers, uint32_t mipLevels, vk::SampleCountFlagBits numSamples) -> GpuAllocation<vk::Image>
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(format);
        imageInfo.setExtent( { static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y), 1 });
        imageInfo.setMipLevels(mipLevels);
        imageInfo.setArrayLayers(arrayLayers);
        imageInfo.setSamples(numSamples);
        imageInfo.setTiling(vk::ImageTiling::eOptimal);
        imageInfo.setUsage(usageFlags);
        imageInfo.setFlags(imageFlags);

        vma::AllocationCreateInfo allocationInfo{};
        allocationInfo.usage = vma::MemoryUsage::eGpuOnly;

        vk::Image image;
        vma::Allocation allocation;
        if (m_allocator.createImage(&imageInfo, &allocationInfo, &image, &allocation, nullptr) != vk::Result::eSuccess)
        {
            throw NcError("Error creating image.");
        }

        return GpuAllocation<vk::Image>{image, allocation, this};
    }

    auto GpuAllocator::CreateTexture(const unsigned char* pixels, uint32_t width, uint32_t height, uint32_t mipLevels, bool isNormal) -> GpuAllocation<vk::Image>
    {
        auto format = isNormal ? vk::Format::eR8G8B8A8Unorm : vk::Format::eR8G8B8A8Srgb;
        vk::FormatProperties formatProperties;
        auto physicalDevice = m_device->VkPhysicalDevice();
        physicalDevice.getFormatProperties(format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
        {
            mipLevels = 1;
        }

        const auto imageSize = width * height * 4u;
        auto stagingBuffer = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly);
        void* mappedData = Map(stagingBuffer.Allocation());
        std::memcpy(mappedData, pixels, imageSize);
        Unmap(stagingBuffer.Allocation());

        auto dimensions = Vector2{static_cast<float>(width), static_cast<float>(height)};
        auto imageAllocation = CreateImage(format, dimensions, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc, vk::ImageCreateFlags(), 1, mipLevels, vk::SampleCountFlagBits::e1);

        TransitionImageLayout(imageAllocation.Data(), vk::ImageLayout::eUndefined, 1, mipLevels, vk::ImageLayout::eTransferDstOptimal);
        CopyBufferToImage(stagingBuffer.Data(), imageAllocation.Data(), width, height);
        stagingBuffer.Release();

        if (mipLevels == 1)
        {
            TransitionImageLayout(imageAllocation.Data(), vk::ImageLayout::eTransferDstOptimal, 1, 1, vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        else
        {
            // Mip mapping transitions the layout to eShaderReadOnlyOptimal
            GenerateMipMaps(imageAllocation.Data(), width, height, mipLevels);
        }

        return imageAllocation;
    }

    auto GpuAllocator::CreateCubeMapTexture(const unsigned char* pixels, uint32_t cubeMapSize, uint32_t sideLength) -> GpuAllocation<vk::Image>
    {
        auto stagingBuffer = CreateBuffer(cubeMapSize, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly);
        auto* mappedData = Map(stagingBuffer.Allocation());
        std::memcpy(mappedData, pixels, cubeMapSize);

        Unmap(stagingBuffer.Allocation());
        auto dimensions = Vector2{static_cast<float>(sideLength), static_cast<float>(sideLength)};
        auto imageBuffer = CreateImage(vk::Format::eR8G8B8A8Srgb, dimensions, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageCreateFlagBits::eCubeCompatible, 6, 1, vk::SampleCountFlagBits::e1);

        TransitionImageLayout(imageBuffer.Data(), vk::ImageLayout::eUndefined, 6, 1, vk::ImageLayout::eTransferDstOptimal);
        CopyBufferToImage(stagingBuffer.Data(), imageBuffer.Data(), sideLength, sideLength, 6);
        TransitionImageLayout(imageBuffer.Data(), vk::ImageLayout::eTransferDstOptimal, 6, 1, vk::ImageLayout::eShaderReadOnlyOptimal);

        stagingBuffer.Release();
        return imageBuffer;
    }

    void GpuAllocator::Destroy(const GpuAllocation<vk::Buffer>& buffer) const
    {
        m_allocator.destroyBuffer(buffer.Data(), buffer.Allocation());
    }

    void GpuAllocator::Destroy(const GpuAllocation<vk::Image>& image) const
    {
        m_allocator.destroyImage(image.Data(), image.Allocation());
    }

    auto GpuAllocator::Map(vma::Allocation allocation) const -> void*
    {
        void* out;
        auto result = m_allocator.mapMemory(allocation, &out);
        if (result != vk::Result::eSuccess)
        {
            throw NcError("Error mapping memory.");
        }
        return out;
    }

    void GpuAllocator::Unmap(vma::Allocation allocation) const
    {
        m_allocator.unmapMemory(allocation);
    }

    void GpuAllocator::GenerateMipMaps(vk::Image image, uint32_t width, uint32_t height, uint32_t mipLevels)
    {
        if (mipLevels <= 1)
        {
            return;
        }

         m_device->ExecuteCommand([&](vk::CommandBuffer cmd) 
        { 
            auto barrier = vk::ImageMemoryBarrier{};
            barrier.setImage(image);
            barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            barrier.subresourceRange.setBaseArrayLayer(0);
            barrier.subresourceRange.setLayerCount(1);
            barrier.subresourceRange.setLevelCount(1);

            auto mipWidth = width;
            auto mipHeight = height;

            for (auto i = 1u; i < mipLevels; i++)
            {
                barrier.subresourceRange.setBaseMipLevel(i - 1);
                barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
                barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
                cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

                auto srcOffsets = std::array<vk::Offset3D, 2>{};
                srcOffsets[0] = vk::Offset3D(0, 0, 0);
                srcOffsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);

                auto srcSubresource = vk::ImageSubresourceLayers{};
                srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
                srcSubresource.setMipLevel(i-1);
                srcSubresource.setLayerCount(1);
                srcSubresource.setBaseArrayLayer(0);

                auto dstOffsets = std::array<vk::Offset3D, 2>{};
                dstOffsets[0] = vk::Offset3D(0, 0, 0);
                dstOffsets[1] = vk::Offset3D(mipWidth > 1 ? mipWidth/2 : 1, mipHeight > 1 ? mipHeight / 2: 1, 1);

                auto dstSubresource = vk::ImageSubresourceLayers{};
                dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
                dstSubresource.setMipLevel(i);
                dstSubresource.setLayerCount(1);
                dstSubresource.setBaseArrayLayer(0);

                auto blit = vk::ImageBlit{};
                blit.setSrcOffsets(srcOffsets);
                blit.setSrcSubresource(srcSubresource);
                blit.setDstSubresource(dstSubresource);
                blit.setDstOffsets(dstOffsets);

                cmd.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

                barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
                barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
                barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

                cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }

            barrier.subresourceRange.setBaseMipLevel(mipLevels-1);
            barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
            barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
            barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }

    void GpuAllocator::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
    {
        m_device->ExecuteCommand([&](vk::CommandBuffer cmd) 
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

    void GpuAllocator::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        m_device->ExecuteCommand([&](vk::CommandBuffer cmd) 
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

    void GpuAllocator::TransitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, uint32_t layerCount, uint32_t mipLevels, vk::ImageLayout newLayout)
    {
        m_device->ExecuteCommand([&](vk::CommandBuffer cmd) 
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
            subresourceRange.setLevelCount(mipLevels);
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
                throw nc::NcError("Unsupported layout transition.");
            }

            cmd.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }

    auto GpuAllocator::CreateTextureView(vk::Image image, uint32_t mipLevels, bool isNormal) -> vk::UniqueImageView
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(image);
        viewInfo.setViewType(vk::ImageViewType::e2D);

        isNormal ? viewInfo.setFormat(vk::Format::eR8G8B8A8Unorm) : viewInfo.setFormat(vk::Format::eR8G8B8A8Srgb);

        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        subresourceRange.setBaseMipLevel(0);
        subresourceRange.setLevelCount(mipLevels);
        subresourceRange.setBaseArrayLayer(0);
        subresourceRange.setLayerCount(1);

        viewInfo.setSubresourceRange(subresourceRange);
        return m_device->VkDevice().createImageViewUnique(viewInfo);
    }

    auto GpuAllocator::CreateCubeMapTextureView(vk::Image image) -> vk::UniqueImageView
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(image);
        viewInfo.setViewType(vk::ImageViewType::eCube);
        viewInfo.setFormat(vk::Format::eR8G8B8A8Srgb);

        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        subresourceRange.setBaseMipLevel(0);
        subresourceRange.setLevelCount(1);
        subresourceRange.setBaseArrayLayer(0);
        subresourceRange.setLayerCount(6);

        viewInfo.setSubresourceRange(subresourceRange);
        return m_device->VkDevice().createImageViewUnique(viewInfo);
    }
} // namespace nc::graphics::vulkan