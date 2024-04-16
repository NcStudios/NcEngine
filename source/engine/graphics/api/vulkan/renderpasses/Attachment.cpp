#include "Attachment.h"

#include "ncutility/NcError.h"

#include <algorithm>

namespace
{
/** Sequence our early frag write after previous (external) frag read. */
constexpr auto earlyFragStencilWriteAfterFragRead = vk::SubpassDependency
{
    VK_SUBPASS_EXTERNAL,
    0,
    vk::PipelineStageFlagBits::eFragmentShader,
    vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlagBits::eShaderRead,
    vk::AccessFlagBits::eDepthStencilAttachmentWrite,
    vk::DependencyFlagBits::eByRegion
};

/** Sequence next (external) frag read after current late frag write. */
constexpr auto lateFragStencilWriteBeforeFragRead = vk::SubpassDependency
{
    0,
    VK_SUBPASS_EXTERNAL,
    vk::PipelineStageFlagBits::eLateFragmentTests,
    vk::PipelineStageFlagBits::eFragmentShader,
    vk::AccessFlagBits::eDepthStencilAttachmentWrite,
    vk::AccessFlagBits::eShaderRead,
    vk::DependencyFlagBits::eByRegion
};

/** Sequence color and stencil writes after previous frame color and early frag. */
constexpr auto colorAndDepthWriteAfterPrevious = vk::SubpassDependency
{
    VK_SUBPASS_EXTERNAL,
    0,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::AccessFlags(),
    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
};

auto CreateAttachmentDescription(nc::graphics::vulkan::AttachmentType type,
                                 vk::Format format,
                                 vk::SampleCountFlagBits numSamples,
                                 vk::AttachmentLoadOp loadOp,
                                 vk::AttachmentStoreOp storeOp) -> vk::AttachmentDescription
{
    using nc::graphics::vulkan::AttachmentType;
    const auto stencilLoadOp = type == AttachmentType::Depth ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
    const auto finalLayout = [](nc::graphics::vulkan::AttachmentType attachmentType)
    {
        switch (attachmentType)
        {
            case AttachmentType::Color:       return vk::ImageLayout::eColorAttachmentOptimal;
            case AttachmentType::Resolve:     return vk::ImageLayout::ePresentSrcKHR;
            case AttachmentType::Depth:       return vk::ImageLayout::eDepthStencilAttachmentOptimal;
            case AttachmentType::ShadowDepth: return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
        }

        throw nc::NcError("Unknown AttachmentType");
    }(type);

    return vk::AttachmentDescription
    {
        vk::AttachmentDescriptionFlags{}, // flags
        format,                           // format
        numSamples,                       // samples
        loadOp,                           // loadOp
        storeOp,                          // storeOp
        stencilLoadOp,                    // stencilLoadOp
        vk::AttachmentStoreOp::eDontCare, // stencilStoreOp
        vk::ImageLayout::eUndefined,      // initialLayout
        finalLayout                       // finalLayout
    };
}

auto CreateAttachmentReference(nc::graphics::vulkan::AttachmentType type, uint32_t attachmentIndex) -> vk::AttachmentReference
{
    using nc::graphics::vulkan::AttachmentType;
    const auto layout = [](AttachmentType attachmentType)
    {
        switch (attachmentType)
        {
            case AttachmentType::Resolve:     [[fallthrough]];
            case AttachmentType::Color:       return vk::ImageLayout::eColorAttachmentOptimal;
            case AttachmentType::Depth:       [[fallthrough]];
            case AttachmentType::ShadowDepth: return vk::ImageLayout::eDepthStencilAttachmentOptimal;
        }
        throw nc::NcError("Unknown AttachmentType");
    }(type);

    return vk::AttachmentReference{attachmentIndex, layout};
}

auto CreateSubpassDescription(const nc::graphics::vulkan::AttachmentSlot& colorAttachment,
                              const nc::graphics::vulkan::AttachmentSlot& depthAttachment,
                              const nc::graphics::vulkan::AttachmentSlot& resolveAttachment) -> vk::SubpassDescription
{
    return vk::SubpassDescription
    {
        vk::SubpassDescriptionFlags{},
        vk::PipelineBindPoint::eGraphics,
        0u,
        nullptr,
        1u,
        &colorAttachment.reference,
        &resolveAttachment.reference,
        &depthAttachment.reference
    };
}

auto CreateSubpassDescription(const nc::graphics::vulkan::AttachmentSlot& depthAttachment) -> vk::SubpassDescription
{
    return vk::SubpassDescription
    {
        vk::SubpassDescriptionFlags{},
        vk::PipelineBindPoint::eGraphics,
        0u,
        nullptr,
        0u,
        nullptr,
        nullptr,
        &depthAttachment.reference
    };
}

auto CreateAttachmentImage(nc::graphics::vulkan::GpuAllocator *allocator, vk::Format format, nc::Vector2 dimensions, vk::SampleCountFlagBits numSamples, bool isDepthStencil) -> nc::graphics::vulkan::GpuAllocation<vk::Image>
{
    constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
    constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
    const auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
    return allocator->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, 1, numSamples);
}

auto CreateAttachmentImageView(vk::Device device, vk::Format format, vk::Image image, bool isDepthStencil) -> vk::UniqueImageView
{
    const auto aspectMask = isDepthStencil ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

    vk::ImageSubresourceRange imageSubresourceRange{};
    imageSubresourceRange.setBaseMipLevel(0);
    imageSubresourceRange.setLevelCount(1);
    imageSubresourceRange.setBaseArrayLayer(0);
    imageSubresourceRange.setLayerCount(1);
    imageSubresourceRange.setAspectMask(aspectMask);

    if (format >= vk::Format::eD16UnormS8Uint && isDepthStencil)
    {
        imageSubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }

    vk::ImageViewCreateInfo imageViewInfo{};
    imageViewInfo.setViewType(vk::ImageViewType::e2D);
    imageViewInfo.setImage(image);
    imageViewInfo.setFormat(format);
    imageViewInfo.setSubresourceRange(imageSubresourceRange);

    return device.createImageViewUnique(imageViewInfo);
}

} // anonymous namespace

namespace nc::graphics::vulkan
{
AttachmentSlot::AttachmentSlot(uint32_t attachmentIndex, AttachmentType attachmentType, vk::Format format, vk::AttachmentLoadOp loadOp,
                               vk::AttachmentStoreOp storeOp, vk::SampleCountFlagBits numSamples)
    : description{CreateAttachmentDescription(attachmentType, format, numSamples, loadOp, storeOp)},
      reference{CreateAttachmentReference(attachmentType, attachmentIndex)},
      type{attachmentType}
{
}

Subpass::Subpass(const AttachmentSlot& colorAttachment, const AttachmentSlot& depthAttachment, const AttachmentSlot& resolveAttachment)
    : description{CreateSubpassDescription(colorAttachment, depthAttachment, resolveAttachment)},
      dependencies{::colorAndDepthWriteAfterPrevious}
{
}

Subpass::Subpass(const AttachmentSlot& depthAttachment)
    : description{CreateSubpassDescription(depthAttachment)},
      dependencies{::earlyFragStencilWriteAfterFragRead, ::lateFragStencilWriteBeforeFragRead}
{
}

Attachment::Attachment(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format format)
    : image{CreateAttachmentImage(allocator, format, dimensions, numSamples, isDepthStencil)},
      view{CreateAttachmentImageView(device, format, image, isDepthStencil)}{}

} // namespace nc::graphics::vulkan
