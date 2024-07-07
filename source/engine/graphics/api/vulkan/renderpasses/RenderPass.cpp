#include "RenderPass.h"

#include "optick.h"

namespace
{
constexpr std::array<float, 4> ClearColor = {0.1f, 0.1f, 0.1f, 0.1f};

auto CreateClearValues(nc::graphics::vulkan::ClearValueFlags_t clearFlags) -> std::vector<vk::ClearValue>
{
    std::vector<vk::ClearValue> clearValues;

    if (clearFlags & nc::graphics::vulkan::ClearValueFlags::Color) clearValues.push_back(vk::ClearValue{vk::ClearColorValue{ClearColor}});
    if (clearFlags & nc::graphics::vulkan::ClearValueFlags::Depth) clearValues.push_back(vk::ClearValue{vk::ClearDepthStencilValue{1.0f, 0}});
    return clearValues;
}

auto GetAttachmentDescriptions(std::span<const nc::graphics::vulkan::AttachmentSlot> slots) -> std::vector<vk::AttachmentDescription>
{
    auto descriptions = std::vector<vk::AttachmentDescription>{};
    descriptions.reserve(slots.size());
    std::ranges::transform(slots, std::back_inserter(descriptions),
                           [](const auto& slot)
                           { return slot.description; });
    return descriptions;
}

auto GetSubpassDescriptions(std::span<const nc::graphics::vulkan::Subpass> subpasses, size_t *dependencyCountOut) -> std::vector<vk::SubpassDescription>
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

auto GetSubpassDependencies(std::span<const nc::graphics::vulkan::Subpass> subpasses, size_t dependencySizeHint = 0ull)
{
    auto dependencies = std::vector<vk::SubpassDependency>{};
    dependencies.reserve(dependencySizeHint);
    for (const auto& subpass : subpasses)
    {
        dependencies.insert(dependencies.cend(), subpass.dependencies.cbegin(), subpass.dependencies.cend());
    }

    return dependencies;
}

auto CreateVkRenderPass(std::span<const nc::graphics::vulkan::AttachmentSlot> attachmentSlots,
                        std::span<const nc::graphics::vulkan::Subpass> subpasses,
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

auto InitSinkViews(nc::graphics::RenderPassSinkType sinkType, std::vector<vk::ImageView>&& renderTargets, vk::ImageLayout layout)
{
    return nc::graphics::vulkan::SinkViews
    {
        sinkType,
        std::move(renderTargets),
        layout
    };
};

} // anonymous namespace

namespace nc::graphics::vulkan
{
RenderPass::RenderPass(vk::Device device,
                       std::span<const AttachmentSlot> attachmentSlots,
                       std::span<const Subpass> subpasses,
                       std::vector<Attachment> attachments,
                       const AttachmentSize &size,
                       ClearValueFlags_t clearFlags)
    : m_device{device},
      m_renderPass{CreateVkRenderPass(attachmentSlots, subpasses, device)},
      m_attachmentSize{size},
      m_clearFlags{clearFlags},
      m_pipelines{},
      m_attachments{std::move(attachments)},
      m_sinkViews{},
      m_sourceSinkPartition{0u}
{
}

RenderPass::RenderPass(vk::Device device,
                       std::span<const AttachmentSlot> attachmentSlots,
                       std::span<const Subpass> subpasses,
                       std::vector<Attachment> attachments,
                       const AttachmentSize &size,
                       ClearValueFlags_t clearFlags,
                       RenderPassSinkType renderTargetsType,
                       std::vector<vk::ImageView>&& renderTargets,
                       vk::ImageLayout renderTargetsLayout,
                       uint32_t sourceSinkPartition)
    : m_device{device},
      m_renderPass{CreateVkRenderPass(attachmentSlots, subpasses, device)},
      m_attachmentSize{size},
      m_clearFlags{clearFlags},
      m_pipelines{},
      m_attachments{std::move(attachments)},
      m_sinkViews{InitSinkViews(renderTargetsType, std::move(renderTargets), renderTargetsLayout)},
      m_sourceSinkPartition{sourceSinkPartition}
{
}

void RenderPass::UnregisterPipelines()
{
    for (auto& pipeline : m_pipelines)
    {
        pipeline.isActive = false;
    }
}

void RenderPass::Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex)
{
    const auto clearValues = CreateClearValues(m_clearFlags);
    const auto renderPassInfo = vk::RenderPassBeginInfo
    {
        m_renderPass.get(),
        m_frameBuffers.at(attachmentIndex).get(),
        vk::Rect2D{vk::Offset2D{0, 0}, m_attachmentSize.extent},
        static_cast<uint32_t>(clearValues.size()),
        clearValues.data()
    };
    cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void RenderPass::Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData, const PerFrameInstanceData& instanceData, uint32_t frameIndex) const
{
    OPTICK_CATEGORY("RenderPass::Execute", Optick::Category::Rendering);

    for (const auto& pipeline : m_pipelines)
    {
        if (pipeline.isActive)
        {
            pipeline.pipeline->Bind(frameIndex, cmd);
            pipeline.pipeline->Record(cmd, frameData, instanceData);
        }
    }
}

void RenderPass::End(vk::CommandBuffer *cmd)
{
    cmd->endRenderPass();
}

auto RenderPass::GetVkPass() const ->vk::RenderPass
{
    return m_renderPass.get();
}

void RenderPass::CreateFrameBuffer(std::span<const vk::ImageView> views, Vector2 dimensions)
{
    const auto framebufferInfo = vk::FramebufferCreateInfo
    {
        vk::FramebufferCreateFlags(),           // FramebufferCreateFlags
        m_renderPass.get(),                     // RenderPass
        static_cast<uint32_t>(views.size()),    // AttachmentCount
        views.data(),                           // PAttachments
        static_cast<uint32_t>(dimensions.x),    // Width
        static_cast<uint32_t>(dimensions.y),    // Height
        1                                       // Layers
    };

    m_frameBuffers.emplace_back(m_device.createFramebufferUnique(framebufferInfo));
}

auto RenderPass::GetSinkViews() const -> std::span<const vk::ImageView>
{
    if (m_sourceSinkPartition >= m_sinkViews.views.size())
    {
        return {};
    }

    return std::span<const vk::ImageView>{m_sinkViews.views.data() + m_sourceSinkPartition, m_sinkViews.views.size() - m_sourceSinkPartition};
}

} // namespace nc::graphics::vulkan
