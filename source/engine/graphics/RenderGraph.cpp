#include "RenderGraph.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuOptions.h"
#include "graphics/PerFrameGpuContext.h"
#include "graphics/Swapchain.h"
#include "graphics/meshes/Meshes.h"
#include "graphics/techniques/EnvironmentTechnique.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/PbrTechnique.h"
#include "graphics/techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/techniques/WireframeTechnique.h"
#endif

#include "optick/optick.h"

#include <string>

namespace
{
constexpr std::array<float, 4> ClearColor = {0.1f, 0.1f, 0.1f, 0.1f};

auto CreateClearValues(nc::graphics::ClearValueFlags_t clearFlags) -> std::vector<vk::ClearValue>
{
    std::vector<vk::ClearValue> clearValues;

    if(clearFlags & nc::graphics::ClearValueFlags::Color) clearValues.push_back(vk::ClearValue{vk::ClearColorValue{ClearColor}});
    if(clearFlags & nc::graphics::ClearValueFlags::Depth) clearValues.push_back(vk::ClearValue{vk::ClearDepthStencilValue{1.0f, 0}});
    return clearValues;
}

void BeginRenderPass(nc::graphics::RenderPass* renderPass, vk::CommandBuffer* cmd, uint32_t attachmentIndex)
{
    const auto clearValues = CreateClearValues(renderPass->clearFlags);
    const auto renderPassInfo = vk::RenderPassBeginInfo
    {
        renderPass->renderPass.get(), 
        nc::graphics::GetFrameBuffer(renderPass, attachmentIndex)->frameBuffer.get(),
        vk::Rect2D{vk::Offset2D{0, 0}, 
        renderPass->attachmentSize.extent},
        static_cast<uint32_t>(clearValues.size()),
        clearValues.data()
    };
    cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void BindMeshBuffers(vk::CommandBuffer* cmd, const nc::graphics::VertexBuffer& vertexData, const nc::graphics::IndexBuffer& indexData)
{
    vk::DeviceSize offsets[] = { 0 };
    auto vertexBuffer = vertexData.buffer.GetBuffer();
    cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
    cmd->bindIndexBuffer(indexData.buffer.GetBuffer(), 0, vk::IndexType::eUint32);
}

void EndRenderPass(vk::CommandBuffer* cmd)
{
    cmd->endRenderPass();
}

void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, nc::Vector2 dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    commandBuffer->setViewport(0, 1, &viewport);
    commandBuffer->setScissor(0, 1, &scissor);
}

auto CreateLitPass(vk::Device device, nc::graphics::GpuAllocator *allocator, nc::graphics::GpuOptions *gpuOptions, nc::graphics::Swapchain *swapchain, nc::Vector2 dimensions) -> std::unique_ptr<nc::graphics::RenderPass>
{
    using namespace nc::graphics;

    auto numSamples = gpuOptions->GetMaxSamplesCount();

    std::vector<AttachmentSlot> litAttachmentSlots{
        AttachmentSlot{0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, numSamples},
        AttachmentSlot{1, AttachmentType::Depth, gpuOptions->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, numSamples},
        AttachmentSlot{2, AttachmentType::Resolve, swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}};

    std::vector<Subpass> litSubpasses{
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}};

    std::vector<std::unique_ptr<Attachment>> attachments;
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, true, numSamples, gpuOptions->GetDepthFormat())); // Depth Stencil
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, false, numSamples, swapchain->GetFormat())); // Color Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = std::make_unique<RenderPass>(device, 1u, LitPassId, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto &colorImageViews = swapchain->GetColorImageViews();
    auto &depthImageView = renderPass->attachments[0]->view;
    auto &colorResolveView = renderPass->attachments[1]->view;

    uint32_t index = 0;
    for (auto &imageView : colorImageViews)
    {
        std::vector<vk::ImageView> imageViews
        {
            colorResolveView.get(), // Color Resolve View
            depthImageView.get(), // Depth View
            imageView.get()
        };
        RegisterAttachments(device, renderPass.get(), imageViews, dimensions, index++);
    }
    return std::move(renderPass);
}
}

namespace nc::graphics
{
RenderGraph::RenderGraph(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions)
    : m_device{device},
      m_swapchain{swapchain},
      m_gpuOptions{gpuOptions},
      m_gpuAllocator{gpuAllocator},
      m_descriptorSets{descriptorSets},
      m_dimensions{dimensions}
{
    Add(CreateLitPass(m_device, m_gpuAllocator, m_gpuOptions, m_swapchain, m_dimensions));

    #ifdef NC_EDITOR_ENABLED
    RegisterTechnique<WireframeTechnique>(LitPassId);
    #endif

    RegisterTechnique<EnvironmentTechnique>(LitPassId);
    RegisterTechnique<PbrTechnique>(LitPassId);
    RegisterTechnique<ParticleTechnique>(LitPassId);
    RegisterTechnique<UiTechnique>(LitPassId);
}

RenderGraph::~RenderGraph() noexcept
{
    m_renderPasses.clear();
}

void RenderGraph::Execute(PerFrameGpuContext *currentFrame, const PerFrameRenderState &frameData, const MeshStorage &meshStorage, uint32_t frameBufferIndex, Vector2 dimensions) const
{
    OPTICK_CATEGORY("RenderGraph::Execute", Optick::Category::Rendering);

    const auto cmd = currentFrame->CommandBuffer();
    SetViewportAndScissor(cmd, dimensions);
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    auto bufferIndex = 0u;
    for (auto &renderPass : m_renderPasses)
    {
        BeginRenderPass(renderPass.get(), cmd, frameBufferIndex);
        for (const auto &technique : renderPass->techniques)
        {
            if (!technique->CanBind(frameData)) continue;

            technique->Bind(cmd);

            if (!technique->CanRecord(frameData)) continue;

            technique->Record(cmd, frameData);
        }
        EndRenderPass(cmd);
    }
    cmd->end();
}

RenderPass* RenderGraph::Acquire(const std::string& uid)
{
    const auto renderPassPos = std::ranges::find_if(m_renderPasses, [uid](auto& renderPass) { return (renderPass->uid == uid); });
    if (renderPassPos == m_renderPasses.end()) throw NcError("RenderGraph::Acquire - Render pass does not exist.");
    return (*renderPassPos).get();
}

void RenderGraph::Add(std::unique_ptr<RenderPass> renderPass)
{
    m_renderPasses.push_back(std::move(renderPass));
    std::ranges::sort(m_renderPasses, [](const auto &renderPassA, const auto &renderPassB)
    {
        return renderPassA->priority < renderPassB->priority;
    });
}

void RenderGraph::Remove(const std::string& uid)
{
    std::erase_if(m_renderPasses, [uid](const auto &pass)
    {
        return pass->uid == uid;
    });
}

void RenderGraph::RegisterShadowMappingTechnique(const std::string& uid, uint32_t shadowCasterIndex)
{
    UnregisterTechnique<ShadowMappingTechnique>(uid);
    auto* renderPass = Acquire(uid);
    renderPass->techniques.push_back(std::make_unique<ShadowMappingTechnique>(m_device, m_gpuOptions, m_descriptorSets, renderPass->renderPass.get(), shadowCasterIndex));
}

void RenderGraph::Resize(Swapchain* swapchain, const Vector2& dimensions)
{
    m_renderPasses.clear();
    m_dimensions = dimensions;
    m_swapchain = swapchain;
    Add(CreateLitPass(m_device, m_gpuAllocator, m_gpuOptions, m_swapchain, m_dimensions));

    #ifdef NC_EDITOR_ENABLED
    RegisterTechnique<WireframeTechnique>(LitPassId);
    #endif

    RegisterTechnique<EnvironmentTechnique>(LitPassId);
    RegisterTechnique<PbrTechnique>(LitPassId);
    RegisterTechnique<ParticleTechnique>(LitPassId);
    RegisterTechnique<UiTechnique>(LitPassId);
}
}