#include "GpuAllocator.h"
#include "Base.h"
#include "debug/NcError.h"

#include "stb/stb_image.h"

namespace
{
    auto CreateAllocator(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::Instance instance) -> vma::Allocator
    {
        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = physicalDevice;
        allocatorInfo.device = logicalDevice;
        allocatorInfo.instance = instance;
        return vma::createAllocator(allocatorInfo);
    }
}

namespace nc::graphics
{
    GpuAllocator::GpuAllocator(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::Instance instance)
        : m_allocator{CreateAllocator(physicalDevice, logicalDevice, instance)},
          m_deviceProperties{physicalDevice.getProperties()}
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

    auto GpuAllocator::CreateImage(vk::Format format, Vector2 dimensions, vk::ImageUsageFlags usageFlags, vk::ImageCreateFlags imageFlags, uint32_t arrayLayers, vk::SampleCountFlagBits numSamples) -> GpuAllocation<vk::Image>
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(format);
        imageInfo.setExtent( { static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y), 1 });
        imageInfo.setMipLevels(1);
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

    auto GpuAllocator::CreateTexture(Base* base, stbi_uc* pixels, uint32_t width, uint32_t height) -> GpuAllocation<vk::Image>
    {
        const auto imageSize = width * height * 4u;
        auto stagingBuffer = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly);
        void* mappedData = Map(stagingBuffer.Allocation());
        std::memcpy(mappedData, pixels, imageSize);
        Unmap(stagingBuffer.Allocation());
        stbi_image_free(pixels);

        auto dimensions = Vector2{static_cast<float>(width), static_cast<float>(height)};
        auto imageAllocation = CreateImage(vk::Format::eR8G8B8A8Srgb, dimensions, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageCreateFlags(), 1, vk::SampleCountFlagBits::e1);

        base->TransitionImageLayout(imageAllocation.Data(), vk::ImageLayout::eUndefined, 1, vk::ImageLayout::eTransferDstOptimal);
        base->CopyBufferToImage(stagingBuffer.Data(), imageAllocation.Data(), width, height);
        base->TransitionImageLayout(imageAllocation.Data(), vk::ImageLayout::eTransferDstOptimal, 1, vk::ImageLayout::eShaderReadOnlyOptimal);

        stagingBuffer.Release();
        return imageAllocation;
    }

    auto GpuAllocator::CreateCubeMapTexture(Base* base, const std::array<stbi_uc*, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize) -> GpuAllocation<vk::Image>
    {
        auto stagingBuffer = CreateBuffer(cubeMapSize, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly);

        auto* mappedData = Map(stagingBuffer.Allocation());

        const uint32_t imageSize = width * height * 4u;

        for(uint32_t layer = 0u; layer < 6u; ++layer)
        {
            char* destination = static_cast<char*>(mappedData) + imageSize * layer;
            std::memcpy(destination, pixels[layer], imageSize);
            stbi_image_free(pixels[layer]);
        }

        Unmap(stagingBuffer.Allocation());
        auto dimensions = Vector2{static_cast<float>(width), static_cast<float>(height)};
        auto imageBuffer = CreateImage(vk::Format::eR8G8B8A8Srgb, dimensions, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageCreateFlagBits::eCubeCompatible, 6, vk::SampleCountFlagBits::e1);

        base->TransitionImageLayout(imageBuffer.Data(), vk::ImageLayout::eUndefined, 6, vk::ImageLayout::eTransferDstOptimal);
        base->CopyBufferToImage(stagingBuffer.Data(), imageBuffer.Data(), width, height, 6);
        base->TransitionImageLayout(imageBuffer.Data(), vk::ImageLayout::eTransferDstOptimal, 6, vk::ImageLayout::eShaderReadOnlyOptimal);

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
        m_allocator.mapMemory(allocation, &out);
        return out;
    }

    void GpuAllocator::Unmap(vma::Allocation allocation) const
    {
        m_allocator.unmapMemory(allocation);
    }

    auto CreateTextureView(vk::Device device, vk::Image image) -> vk::UniqueImageView
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
        return device.createImageViewUnique(viewInfo);
    }

    auto CreateCubeMapTextureView(vk::Device device, vk::Image image) -> vk::UniqueImageView
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
        return device.createImageViewUnique(viewInfo);
    }
} // namespace nc::graphics