#include "RenderGraph.h"
#include "FrameManager.h"
#include "GpuAllocator.h"
#include "PerFrameGpuContext.h"
#include "Swapchain.h"
#include "core/Device.h"
#include "techniques/EnvironmentTechnique.h"
#include "techniques/OutlineTechnique.h"
#include "techniques/ParticleTechnique.h"
#include "techniques/PbrTechnique.h"
#include "techniques/ToonTechnique.h"
#include "techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "techniques/WireframeTechnique.h"
#endif

#include "optick.h"

#include <array>
#include <string>

namespace
{
void BindMeshBuffers(vk::CommandBuffer* cmd, const nc::graphics::ImmutableBuffer& vertexData, const nc::graphics::ImmutableBuffer& indexData)
{
    vk::DeviceSize offsets[] = { 0 };
    auto vertexBuffer = vertexData.GetBuffer();
    cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
    cmd->bindIndexBuffer(indexData.GetBuffer(), 0, vk::IndexType::eUint32);
}

void SetViewportAndScissor(vk::CommandBuffer* cmd, nc::Vector2 dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    cmd->setViewport(0, 1, &viewport);
    cmd->setScissor(0, 1, &scissor);
}

auto CreateLitPass(const nc::graphics::Device& device, nc::graphics::GpuAllocator *allocator, nc::graphics::Swapchain *swapchain, nc::Vector2 dimensions) -> nc::graphics::RenderPass
{
    using namespace nc::graphics;

    const auto vkDevice = device.VkDevice();
    const auto& gpuOptions = device.GetGpuOptions();

    auto numSamples = gpuOptions.GetMaxSamplesCount();

    const auto litAttachmentSlots = std::array<AttachmentSlot, 3>
    {
        AttachmentSlot{0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, numSamples},
        AttachmentSlot{1, AttachmentType::Depth, gpuOptions.GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, numSamples},
        AttachmentSlot{2, AttachmentType::Resolve, swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    const auto litSubpasses = std::array<Subpass, 1>
    {
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
    };

    std::vector<Attachment> attachments;
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, numSamples, gpuOptions.GetDepthFormat())); // Depth Stencil
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, false, numSamples, swapchain->GetFormat())); // Color Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, 1u, LitPassId, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto &colorImageViews = swapchain->GetColorImageViews();
    auto depthImageView = renderPass.GetAttachmentView(0);
    auto colorResolveView = renderPass.GetAttachmentView(1);

    uint32_t index = 0;
    for (auto &imageView : colorImageViews)
    {
        std::vector<vk::ImageView> imageViews
        {
            colorResolveView, // Color Resolve View
            depthImageView, // Depth View
            imageView.get()
        };
        renderPass.RegisterAttachmentViews(imageViews, dimensions, index++);
    }

    return renderPass;
}
}

namespace nc::graphics
{
RenderGraph::RenderGraph(const Device& device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions)
    : m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_descriptorSets{descriptorSets},
      m_dimensions{dimensions}
{
    auto litPass = CreateLitPass(device, m_gpuAllocator, m_swapchain, m_dimensions);

    #ifdef NC_EDITOR_ENABLED
    litPass.RegisterTechnique<WireframeTechnique>(device, m_descriptorSets);
    #endif

    litPass.RegisterTechnique<EnvironmentTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<PbrTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<ToonTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<OutlineTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<ParticleTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<UiTechnique>(device, m_descriptorSets);
    AddRenderPass(std::move(litPass));
}

void RenderGraph::Execute(PerFrameGpuContext *currentFrame, const PerFrameRenderState &frameData, const MeshStorage &meshStorage, uint32_t frameBufferIndex, Vector2 dimensions)
{
    OPTICK_CATEGORY("RenderGraph::Execute", Optick::Category::Rendering);

    const auto cmd = currentFrame->CommandBuffer();
    SetViewportAndScissor(cmd, dimensions);
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    for (auto& renderPass : m_renderPasses)
    {
        renderPass.Begin(cmd, frameBufferIndex);
        renderPass.Execute(cmd, frameData);
        renderPass.End(cmd);
    }
    cmd->end();
}

auto RenderGraph::GetRenderPass(const std::string& uid) -> const RenderPass&
{
    const auto renderPassPos = std::ranges::find_if(m_renderPasses, [uid](auto &renderPass)
    {
        return renderPass.GetUid() == uid;
    });

    if (renderPassPos == m_renderPasses.end()) throw NcError("RenderGraph::GetRenderPass - Render pass does not exist.");
    return *renderPassPos;
}

void RenderGraph::AddRenderPass(RenderPass renderPass)
{
    m_renderPasses.push_back(std::move(renderPass));
    std::ranges::sort(m_renderPasses, [](auto &renderPassA, auto &renderPassB)
    {
        return renderPassA.GetPriority() < renderPassB.GetPriority();
    });
}

void RenderGraph::RemoveRenderPass(const std::string& uid)
{
    std::erase_if(m_renderPasses, [uid](const auto &renderPass)
    {
        return renderPass.GetUid() == uid;
    });
}

void RenderGraph::Resize(const Device& device, const Vector2& dimensions)
{
    m_renderPasses.clear();
    m_dimensions = dimensions;

    auto litPass = CreateLitPass(device, m_gpuAllocator, m_swapchain, m_dimensions);

    #ifdef NC_EDITOR_ENABLED
    litPass.RegisterTechnique<WireframeTechnique>(device, m_descriptorSets);
    #endif

    litPass.RegisterTechnique<EnvironmentTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<PbrTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<ToonTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<OutlineTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<ParticleTechnique>(device, m_descriptorSets);
    litPass.RegisterTechnique<UiTechnique>(device, m_descriptorSets);
    AddRenderPass(std::move(litPass));
}
}
