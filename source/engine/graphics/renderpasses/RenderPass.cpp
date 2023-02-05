#include "RenderPass.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuOptions.h"
#include "graphics/Swapchain.h"

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

auto CreateAttachmentDescription(nc::graphics::AttachmentType type,
                                 vk::Format format,
                                 vk::SampleCountFlagBits numSamples,
                                 vk::AttachmentLoadOp loadOp,
                                 vk::AttachmentStoreOp storeOp) -> vk::AttachmentDescription
{
    using nc::graphics::AttachmentType;
    const auto stencilLoadOp = type == AttachmentType::Depth ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
    const auto finalLayout = [](nc::graphics::AttachmentType attachmentType)
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

auto CreateAttachmentReference(nc::graphics::AttachmentType type, uint32_t attachmentIndex) -> vk::AttachmentReference
{
    using nc::graphics::AttachmentType;
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

auto CreateSubpassDescription(const nc::graphics::AttachmentSlot& colorAttachment,
                              const nc::graphics::AttachmentSlot& depthAttachment,
                              const nc::graphics::AttachmentSlot& resolveAttachment) -> vk::SubpassDescription
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

auto CreateSubpassDescription(const nc::graphics::AttachmentSlot& depthAttachment) -> vk::SubpassDescription
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

auto GetAttachmentDescriptions(std::span<const nc::graphics::AttachmentSlot> slots) -> std::vector<vk::AttachmentDescription>
{
    auto descriptions = std::vector<vk::AttachmentDescription>{};
    descriptions.reserve(slots.size());
    std::ranges::transform(slots, std::back_inserter(descriptions),
                           [](const auto& slot){ return slot.description; });
    return descriptions;
}

auto GetSubpassDescriptions(std::span<const nc::graphics::Subpass> subpasses, size_t* dependencyCountOut) -> std::vector<vk::SubpassDescription>
{
    auto depCount = size_t{0ull};
    auto descriptions = std::vector<vk::SubpassDescription>{};
    descriptions.reserve(subpasses.size());
    std::ranges::transform(subpasses, std::back_inserter(descriptions), [&depCount](const auto& subpass)
    {
        depCount += subpass.dependencies.size();
        return subpass.description;
    });

    *dependencyCountOut = depCount;
    return descriptions;
}

auto GetSubpassDependencies(std::span<const nc::graphics::Subpass> subpasses, size_t dependencySizeHint = 0ull)
{
    auto dependencies = std::vector<vk::SubpassDependency>{};
    dependencies.reserve(dependencySizeHint);
    for (const auto& subpass : subpasses)
    {
        dependencies.insert(dependencies.cend(), subpass.dependencies.cbegin(), subpass.dependencies.cend());
    }

    return dependencies;
}

auto CreateRenderPass(std::span<const nc::graphics::AttachmentSlot> attachmentSlots,
                      std::span<const nc::graphics::Subpass> subpasses,
                      vk::Device device) -> vk::UniqueRenderPass
{
    const auto attachmentDescriptions = GetAttachmentDescriptions(attachmentSlots);
    auto subpassDependencyCount = size_t{0ull};
    const auto subpassDescriptions = GetSubpassDescriptions(subpasses, &subpassDependencyCount);
    const auto subpassDependencies = GetSubpassDependencies(subpasses, subpassDependencyCount);
    const auto renderPassInfo = vk::RenderPassCreateInfo
    {
        vk::RenderPassCreateFlags{}, static_cast<uint32_t>(attachmentDescriptions.size()),
        attachmentDescriptions.data(), static_cast<uint32_t>(subpassDescriptions.size()),
        subpassDescriptions.data(), static_cast<uint32_t>(subpassDependencies.size()),
        subpassDependencies.data()
    };

    return device.createRenderPassUnique(renderPassInfo);
}

void RegisterAttachments(vk::Device device, nc::graphics::RenderPass* renderPass, std::span<const vk::ImageView> attachmentHandles, uint32_t index)
{
    auto frameBufferPos = std::ranges::find_if(renderPass->frameBuffers, [index](const auto& frameBuffer)
    {
        return (frameBuffer->index == index);
    });

    if (frameBufferPos != renderPass->frameBuffers.end())
    {
        *frameBufferPos = std::move(renderPass->frameBuffers.back());
        renderPass->frameBuffers.pop_back();
    }

    auto frameBuffer = std::make_unique<nc::graphics::FrameBuffer>();

    frameBuffer->attachmentHandles = std::move(attachmentHandles);
    frameBuffer->index = index;

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass->renderPass.get());
    framebufferInfo.setAttachmentCount(static_cast<uint32_t>(frameBuffer->attachmentHandles.size()));
    framebufferInfo.setPAttachments(frameBuffer->attachmentHandles.data());
    framebufferInfo.setWidth(static_cast<uint32_t>(renderPass->attachmentSize.dimensions.x));
    framebufferInfo.setHeight(static_cast<uint32_t>(renderPass->attachmentSize.dimensions.y));
    framebufferInfo.setLayers(1);

    frameBuffer->frameBuffer = device.createFramebufferUnique(framebufferInfo);
    renderPass->frameBuffers.push_back(std::move(frameBuffer));
}

void RegisterAttachment(vk::Device device, nc::graphics::RenderPass* renderPass, vk::ImageView attachmentHandle, uint32_t index)
{
    auto frameBufferPos = std::ranges::find_if(renderPass->frameBuffers, [index](const auto& frameBuffer)
    {
        return (frameBuffer->index == index);
    });

    if (frameBufferPos != renderPass->frameBuffers.end())
    {
        *frameBufferPos = std::move(renderPass->frameBuffers.back());
        renderPass->frameBuffers.pop_back();
    }

    auto frameBuffer = std::make_unique<nc::graphics::FrameBuffer>();

    auto handles = std::vector<vk::ImageView>{attachmentHandle};
    frameBuffer->attachmentHandles = handles;
    frameBuffer->index = index;

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass->renderPass.get());
    framebufferInfo.setAttachmentCount(1);
    framebufferInfo.setPAttachments(frameBuffer->attachmentHandles.data());
    framebufferInfo.setWidth(static_cast<uint32_t>(renderPass->attachmentSize.dimensions.x));
    framebufferInfo.setHeight(static_cast<uint32_t>(renderPass->attachmentSize.dimensions.y));
    framebufferInfo.setLayers(1);

    frameBuffer->frameBuffer = device.createFramebufferUnique(framebufferInfo);
    renderPass->frameBuffers.push_back(std::move(frameBuffer));
}
} // anonymous namespace

namespace nc::graphics
{
AttachmentSlot::AttachmentSlot(uint32_t attachmentIndex, AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp,
                               vk::AttachmentStoreOp storeOp, vk::SampleCountFlagBits numSamples)
    : description{CreateAttachmentDescription(type, format, numSamples, loadOp, storeOp)},
      reference{CreateAttachmentReference(type, attachmentIndex)},
      type{type}
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

auto CreateImage(GpuAllocator* allocator, vk::Format format, Vector2 dimensions, vk::SampleCountFlagBits numSamples, bool isDepthStencil) -> nc::graphics::GpuAllocation<vk::Image>
{
    constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
    constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
    const auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
    return allocator->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, numSamples);
}

auto CreateImageView(vk::Device device, vk::Format format, vk::Image image,bool isDepthStencil) -> vk::UniqueImageView
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

Attachment::Attachment(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format format)
    : image{CreateImage(allocator, format, dimensions, numSamples, isDepthStencil)},
      view{CreateImageView(device, format, image, isDepthStencil)}
    {}

RenderPass::RenderPass(vk::Device device,
                       uint8_t priority_,
                       std::string uid_,
                       std::vector<AttachmentSlot> attachmentSlots,
                       std::vector<Subpass> subpasses,
                       std::vector<std::unique_ptr<Attachment>> attachments_,
                       const AttachmentSize& size,
                       ClearValueFlags_t clearFlags_)
    : priority{priority_},
      uid{std::move(uid_)},
      renderPass{std::move(CreateRenderPass(attachmentSlots, subpasses, device))},
      attachmentSize{size},
      clearFlags{clearFlags_},
      techniques{},
      attachments{std::move(attachments_)},
      frameBuffers{}
{};

nc::graphics::FrameBuffer* GetFrameBuffer(nc::graphics::RenderPass* renderPass, uint32_t index)
{
    auto frameBufferPos = std::ranges::find_if(renderPass->frameBuffers, [index](const auto& frameBuffer)
    {
        return (frameBuffer->index == index);
    });

    if (frameBufferPos == renderPass->frameBuffers.end())
    {
        return renderPass->frameBuffers[0].get();
    }

    return frameBufferPos->get();
}

auto CreateFrameBuffer(vk::Device device, vk::RenderPass renderPass, std::span<const vk::ImageView> attachmentHandles, uint32_t width, uint32_t height) -> vk::UniqueFramebuffer
{
    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass);
    framebufferInfo.setAttachmentCount(static_cast<uint32_t>(attachmentHandles.size()));
    framebufferInfo.setPAttachments(attachmentHandles.data());
    framebufferInfo.setWidth(width);
    framebufferInfo.setHeight(height);
    framebufferInfo.setLayers(1);
    return device.createFramebufferUnique(framebufferInfo);
}

auto CreateLitPass(vk::Device device, nc::graphics::GpuAllocator* allocator, nc::graphics::GpuOptions* gpuOptions, nc::graphics::Swapchain* swapchain, nc::Vector2 dimensions) -> std::unique_ptr<nc::graphics::RenderPass>
{
    using namespace nc::graphics;

    auto numSamples = gpuOptions->GetMaxSamplesCount();

    std::vector<AttachmentSlot> litAttachmentSlots
    {
        AttachmentSlot{0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, numSamples},
        AttachmentSlot{1, AttachmentType::Depth, gpuOptions->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, numSamples},
        AttachmentSlot{2, AttachmentType::Resolve, swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    std::vector<Subpass> litSubpasses
    {
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
    };

    std::vector<std::unique_ptr<Attachment>> attachments;
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, true, numSamples, gpuOptions->GetDepthFormat())); // Depth Stencil
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, false, numSamples, swapchain->GetFormat())); // Color Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = std::make_unique<RenderPass>(device, 1u, LitPassId, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto& colorImageViews = swapchain->GetColorImageViews();
    auto& depthImageView = renderPass->attachments[0]->view;
    auto& colorResolveView = renderPass->attachments[1]->view;

    uint32_t index = 0;
    for (auto& imageView : colorImageViews) 
    {
        std::vector<vk::ImageView> imageViews
        {
            colorResolveView.get(), // Color Resolve View
            depthImageView.get(), // Depth View
            imageView
        };
        RegisterAttachments(device, renderPass.get(), imageViews, index++); 
    }
    return std::move(renderPass);
}

auto CreateShadowMappingPass(vk::Device device, nc::graphics::GpuAllocator* allocator, nc::graphics::GpuOptions* gpuOptions, nc::graphics::Swapchain* swapchain, nc::Vector2 dimensions, uint32_t index) -> std::unique_ptr<nc::graphics::RenderPass>
{
    using namespace nc::graphics;
    auto id = ShadowMappingPassId + std::to_string(index);

    std::vector<AttachmentSlot> shadowAttachmentSlots
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    std::vector<Subpass> shadowSubpasses
    {
        Subpass{shadowAttachmentSlots[0]}
    };

    std::vector<std::unique_ptr<Attachment>> attachments;
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = std::make_unique<RenderPass>(device, 0u, id, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth);

    RegisterAttachment(device, renderPass.get(), renderPass->attachments[0]->view.get(), 0);

    return std::move(renderPass);
   }
} // namespace nc::graphics
