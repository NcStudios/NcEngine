#include "RenderGraph.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuOptions.h"
#include "graphics/meshes/Meshes.h"
#include "graphics/PerFrameGpuContext.h"
#include "graphics/Swapchain.h"
#include "graphics/techniques/EnvironmentTechnique.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/PbrTechnique.h"
#include "graphics/techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/techniques/WireframeTechnique.h"
#endif

#include "optick/optick.h"

#include <iostream>
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
        GetFrameBuffer(renderPass, attachmentIndex)->frameBuffer.get(),
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
}

namespace nc::graphics
{
RenderGraph::RenderGraph(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions)
    : m_device{device},
      m_swapchain{swapchain},
      m_gpuOptions{gpuOptions},
      m_gpuAllocator{gpuAllocator},
      m_descriptorSets{descriptorSets},
      m_renderPasses{},
      m_dimensions{dimensions}
{
    Add(CreateLitPass(device, gpuAllocator, gpuOptions, swapchain, m_dimensions));

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

void RenderGraph::Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, const MeshStorage& meshStorage, uint32_t frameBufferIndex, Vector2 dimensions)
{
    OPTICK_CATEGORY("RenderGraph::Execute", Optick::Category::Rendering);

    auto cmd = currentFrame->CommandBuffer();
    SetViewportAndScissor(cmd, dimensions);
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    uint32_t bufferIndex = 0u;
    for (auto& renderPass : m_renderPasses)
    {
        BeginRenderPass(renderPass.get(), cmd, frameBufferIndex);
        for (auto& technique : renderPass->techniques)
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
    auto renderPassPos = std::ranges::find_if(m_renderPasses, [uid](auto& renderPass) { return (renderPass->uid == uid); });
    if (renderPassPos == m_renderPasses.end()) throw NcError("RenderGraph::Acquire - Render pass does not exist.");
    return (*renderPassPos).get();
}

void RenderGraph::Add(std::unique_ptr<RenderPass> renderPass)
{
    m_renderPasses.push_back(std::move(renderPass));
    std::sort(m_renderPasses.begin(), m_renderPasses.end(), [](const auto& renderPassA, const auto& renderPassB) {return renderPassA->priority < renderPassB->priority;});
}

void RenderGraph::Remove(const std::string& uid)
{
    const auto it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), ([uid](const auto& pass){return pass->uid == uid;}));
    if(it == m_renderPasses.end()) throw NcError("RenderGraph::Remove - Render pass does not exist: " + uid);

    *it = std::move(m_renderPasses.back());
    m_renderPasses.pop_back();
}

void RenderGraph::RegisterShadowMappingTechnique(const std::string& uid, uint32_t shadowCasterIndex)
{
    UnregisterTechnique<ShadowMappingTechnique>(uid);
    auto* renderPass = Acquire(uid);
    renderPass->techniques.push_back(std::make_unique<ShadowMappingTechnique>(m_device, m_gpuOptions, m_descriptorSets, renderPass->renderPass.get(), shadowCasterIndex));
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    m_renderPasses.clear();
    m_dimensions = dimensions;
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