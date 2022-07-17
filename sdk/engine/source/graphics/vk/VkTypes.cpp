#include "VkTypes.h"

#include <algorithm>

namespace
{
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
    vk::SubpassDescription subpassDescription{};
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
    subpassDescription.setColorAttachmentCount(1);
    subpassDescription.setPColorAttachments(&colorAttachment.reference);
    subpassDescription.setPDepthStencilAttachment(&depthAttachment.reference);
    subpassDescription.setPResolveAttachments(&resolveAttachment.reference);
    return subpassDescription;
}

auto CreateSubpassDescription(const nc::graphics::AttachmentSlot& depthAttachment) -> vk::SubpassDescription
{
    vk::SubpassDescription subpassDescription{};
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
    subpassDescription.setColorAttachmentCount(0);
    subpassDescription.setPDepthStencilAttachment(&depthAttachment.reference);
    return subpassDescription;
}

auto CreateSubpassDependency(uint32_t sourceSubpassIndex,
                             uint32_t destSubpassIndex,
                             vk::PipelineStageFlags sourceStageMask,
                             vk::PipelineStageFlags destStageMask,
                             vk::AccessFlags sourceAccessMask,
                             vk::AccessFlags destAccessMask,
                             vk::DependencyFlags flags = vk::DependencyFlags{}) -> vk::SubpassDependency
{
    return vk::SubpassDependency
    {
        sourceSubpassIndex, // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
        destSubpassIndex,   // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
        sourceStageMask,    // The type of operation to wait on. (What our dependency is)
        destStageMask,      // Specifies the type of operation that should do the waiting
        sourceAccessMask,   // Specifies the specific operation that should do the waiting
        destAccessMask,     // Specifies the specific operation that should do the waiting
        flags
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

Subpass::Subpass(const AttachmentSlot& depthAttachment)
    : description{CreateSubpassDescription(depthAttachment)},
      dependencies{CreateSubpassDependency(VK_SUBPASS_EXTERNAL, 0,
                                             vk::PipelineStageFlagBits::eFragmentShader,
                                             vk::PipelineStageFlagBits::eEarlyFragmentTests,
                                             vk::AccessFlagBits::eShaderRead,
                                             vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                                             vk::DependencyFlagBits::eByRegion),
                    CreateSubpassDependency(0, VK_SUBPASS_EXTERNAL,
                                            vk::PipelineStageFlagBits::eLateFragmentTests,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                                            vk::AccessFlagBits::eShaderRead,
                                            vk::DependencyFlagBits::eByRegion)}
{
}

Subpass::Subpass(const AttachmentSlot& colorAttachment, const AttachmentSlot& depthAttachment, const AttachmentSlot& resolveAttachment)
    : description{CreateSubpassDescription(colorAttachment, depthAttachment, resolveAttachment)},
      dependencies{CreateSubpassDependency(VK_SUBPASS_EXTERNAL, 0,
                                             vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
                                             vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
                                             vk::AccessFlags(),
                                             vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)}
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
