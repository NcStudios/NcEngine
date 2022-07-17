#include "VkTypes.h"

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

RenderPass::RenderPass(std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses,
                       vk::Device device, const RenderTargetSize& size, std::string uid_, ClearValueFlags_t clearFlags_)
    : renderpass{CreateRenderPass(attachmentSlots, subpasses, device)},
      techniques{},
      renderTargetSize{size},
      uid{std::move(uid_)},
      clearFlags{clearFlags_}
{
}
} // namespace nc::graphics
