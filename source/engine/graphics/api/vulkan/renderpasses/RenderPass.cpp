#include "RenderPass.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"

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
} // anonymous namespace

namespace nc::graphics::vulkan
{
RenderPass::RenderPass(vk::Device device,
                       std::string uid,
                       std::span<const AttachmentSlot> attachmentSlots,
                       std::span<const Subpass> subpasses,
                       std::vector<Attachment> attachments,
                       const AttachmentSize &size,
                       ClearValueFlags_t clearFlags)
    : m_device{device},
      m_uid{std::move(uid)},
      m_renderPass{CreateVkRenderPass(attachmentSlots, subpasses, device)},
      m_attachmentSize{size},
      m_clearFlags{clearFlags},
      m_attachments{std::move(attachments)}
{
}

void RenderPass::RegisterShadowMappingTechnique(vk::Device device, ShaderBindingManager* shaderBindingManager, uint32_t shadowCasterIndex)
{
    m_shadowMappingTechniques.push_back(std::make_unique<ShadowMappingTechnique>(device, shaderBindingManager, m_renderPass.get(), shadowCasterIndex));
}

void RenderPass::UnregisterShadowMappingTechnique()
{
    if (!m_shadowMappingTechniques.empty())
        m_shadowMappingTechniques.pop_back();
}

void RenderPass::Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex)
{
    const auto clearValues = CreateClearValues(m_clearFlags);
    const auto renderPassInfo = vk::RenderPassBeginInfo
    {
        m_renderPass.get(),
        GetFrameBuffer(attachmentIndex),
        vk::Rect2D{vk::Offset2D{0, 0}, m_attachmentSize.extent},
        static_cast<uint32_t>(clearValues.size()),
        clearValues.data()
    };
    cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void RenderPass::Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData, uint32_t frameIndex) const
{
    OPTICK_CATEGORY("RenderPass::Execute", Optick::Category::Rendering);

    for (const auto &technique : m_shadowMappingTechniques)
    {
        if (!technique->CanBind(frameData)) continue;
        technique->Bind(frameIndex, cmd);

        if (!technique->CanRecord(frameData)) continue;
        technique->Record(cmd, frameData);
    }

    for (const auto &technique : m_litTechniques)
    {
        if (!technique->CanBind(frameData)) continue;
        technique->Bind(frameIndex, cmd);

        if (!technique->CanRecord(frameData)) continue;
        technique->Record(cmd, frameData);
    }
}

void RenderPass::End(vk::CommandBuffer *cmd)
{
    cmd->endRenderPass();
}

auto RenderPass::GetAttachmentView(uint32_t index) const -> vk::ImageView
{
    return m_attachments.at(index).view.get();
}

auto RenderPass::GetUid() const -> std::string
{
    return m_uid;
}

auto RenderPass::GetVkPass() const ->vk::RenderPass
{
    return m_renderPass.get();
}

void RenderPass::CreateFrameBuffers(std::span<const vk::ImageView> views, Vector2 dimensions, uint32_t index)
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

    m_frameBuffers.emplace_back(index, m_device.createFramebufferUnique(framebufferInfo));
}

auto RenderPass::GetFrameBuffer(uint32_t index) -> vk::Framebuffer
{
    const auto frameBufferPos = std::ranges::find_if(m_frameBuffers, [index](const auto &frameBuffer)
    {
        return (frameBuffer.index == index);
    });

    if (frameBufferPos == m_frameBuffers.end())
    {
        return m_frameBuffers.at(0).frameBuffer.get();
    }
    return frameBufferPos->frameBuffer.get();
}
} // namespace nc::graphics::vulkan
