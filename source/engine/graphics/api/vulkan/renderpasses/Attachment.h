#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

#include "ncmath/Vector.h"

#include <memory>
#include <span>
#include <vector>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
using ClearValueFlags_t = uint8_t;

struct ClearValueFlags
{
    static constexpr ClearValueFlags_t None = 0;
    static constexpr ClearValueFlags_t Color = 1 << 1;
    static constexpr ClearValueFlags_t Depth = 1 << 2;
};

struct Attachment
{
    Attachment(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format depthFormat);
    GpuAllocation<vk::Image> image;
    vk::UniqueImageView view;
};

struct AttachmentSlot
{
    AttachmentSlot(uint32_t attachmentIndex,
                   vk::Format format_,
                   vk::ImageLayout initialLayout,
                   vk::ImageLayout subpassLayout,
                   vk::ImageLayout finalLayout,
                   vk::AttachmentLoadOp loadOp,
                   vk::AttachmentStoreOp storeOp,
                   vk::AttachmentLoadOp stencilLoadOp,
                   vk::AttachmentStoreOp stencilStoreOp,
                   vk::SampleCountFlagBits numSamples_);
    vk::AttachmentDescription description;
    vk::AttachmentReference reference;
    vk::SampleCountFlagBits numSamples;
    vk::Format format;
};

struct Subpass
{
    Subpass(const AttachmentSlot &colorAttachment, const AttachmentSlot &depthAttachment, const AttachmentSlot &resolveAttachment);
    explicit Subpass(const AttachmentSlot &depthAttachment);
    vk::SubpassDescription description;
    std::vector<vk::SubpassDependency> dependencies{};
};

struct AttachmentSize
{
    Vector2 dimensions;
    vk::Extent2D extent;
};
} // namespace nc::graphics::vulkan
