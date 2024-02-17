#include "RenderGraph.h"
#include "FrameManager.h"
#include "GpuAllocator.h"
#include "PerFrameGpuContext.h"
#include "Swapchain.h"
#include "core/Device.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"
#include "graphics/GraphicsUtilities.h"
#include "techniques/EnvironmentTechnique.h"
#include "techniques/OutlineTechnique.h"
#include "techniques/ParticleTechnique.h"
#include "techniques/PbrTechnique.h"
#include "techniques/ShadowMappingTechnique.h"
#include "techniques/ToonTechnique.h"
#include "techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "techniques/WireframeTechnique.h"
#endif

#include "optick.h"

#include <array>
#include <string>
#include <ranges>

#include <iostream>

namespace
{
void BindMeshBuffers(vk::CommandBuffer* cmd, const nc::graphics::vulkan::MeshBuffer& vertexData, const nc::graphics::vulkan::MeshBuffer& indexData)
{
    vk::DeviceSize offsets[] = { 0 };
    auto vertexBuffer = vertexData.GetBuffer();
    cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
    cmd->bindIndexBuffer(indexData.GetBuffer(), 0, vk::IndexType::eUint32);
}

void SetViewportAndScissorFullWindow(vk::CommandBuffer* cmd, const nc::Vector2& dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    cmd->setViewport(0, 1, &viewport);
    cmd->setScissor(0, 1, &scissor);
}

void SetViewportAndScissorAspectRatio(vk::CommandBuffer* cmd, const nc::Vector2& dimensions, const nc::Vector2& screenExtent)
{
    const auto viewport = vk::Viewport{(dimensions.x - screenExtent.x) / 2, (dimensions.y - screenExtent.y) / 2, screenExtent.x, screenExtent.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(screenExtent.x), static_cast<uint32_t>(screenExtent.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{static_cast<int32_t>(dimensions.x - static_cast<float>(extent.width)) / 2, static_cast<int32_t>(dimensions.y - static_cast<float>(extent.height)) / 2}, extent};
    cmd->setViewport(0, 1, &viewport);
    cmd->setScissor(0, 1, &scissor);
}

auto CreateShadowMappingPass(const nc::graphics::Device* device, nc::graphics::GpuAllocator* allocator, nc::graphics::Swapchain* swapchain, const nc::Vector2& dimensions, uint32_t index) -> nc::graphics::RenderPass
{
    using namespace nc::graphics;

    const auto vkDevice = device->VkDevice();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    std::vector<Attachment> attachments;
    const auto numConcurrentAttachments = swapchain->GetColorImageViews().size();
    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
    }

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, 0u, ShadowMappingPassId + std::to_string(index), shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth);

    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        const auto views = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(i)};
        renderPass.RegisterAttachmentViews(views, dimensions, i);
    }

    return renderPass;
}

auto CreateLitPass(const nc::graphics::Device* device, nc::graphics::GpuAllocator* allocator, nc::graphics::Swapchain* swapchain, const nc::Vector2& dimensions) -> nc::graphics::RenderPass
{
    using namespace nc::graphics;

    const auto vkDevice = device->VkDevice();
    const auto& gpuOptions = device->GetGpuOptions();

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
RenderGraph::RenderGraph(const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions, uint32_t maxLights)
    : m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_descriptorSets{descriptorSets},
      m_shadowMappingPasses{},
      m_litPass{CreateLitPass(device, m_gpuAllocator, m_swapchain, dimensions)},
      m_dimensions{dimensions},
      m_screenExtent{},
      m_activeShadowMappingPasses{},
      m_maxLights{maxLights},
      m_isDescriptorSetLayoutsDirty{true},
      m_onDescriptorSetsChanged{m_descriptorSets->OnResourceLayoutChanged().Connect(this, &RenderGraph::SetDescriptorSetLayoutsDirty)}
{
    for (auto i : std::views::iota(0u, m_maxLights))
    {
        m_shadowMappingPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i));
    }
}

void RenderGraph::MapShaderResources()
{
    for (auto i : std::views::iota(0u, m_activeShadowMappingPasses))
    {
        m_shadowMappingPasses[i].ClearTechniques();
        m_shadowMappingPasses[i].RegisterShadowMappingTechnique(m_device->VkDevice(), m_descriptorSets, i);
    }

    m_litPass.ClearTechniques();

    #ifdef NC_EDITOR_ENABLED
    m_litPass.RegisterTechnique<WireframeTechnique>(*m_device, m_descriptorSets);
    #endif

    m_litPass.RegisterTechnique<EnvironmentTechnique>(*m_device, m_descriptorSets);
    m_litPass.RegisterTechnique<PbrTechnique>(*m_device, m_descriptorSets);
    m_litPass.RegisterTechnique<ToonTechnique>(*m_device, m_descriptorSets);
    m_litPass.RegisterTechnique<OutlineTechnique>(*m_device, m_descriptorSets);
    m_litPass.RegisterTechnique<ParticleTechnique>(*m_device, m_descriptorSets);
    m_litPass.RegisterTechnique<UiTechnique>(*m_device, m_descriptorSets);
}

void RenderGraph::Execute(PerFrameGpuContext *currentFrame, const PerFrameRenderState &frameData, const vulkan::MeshStorage &meshStorage, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent, uint32_t frameIndex)
{
    OPTICK_CATEGORY("RenderGraph::Execute", Optick::Category::Rendering);

    if (m_isDescriptorSetLayoutsDirty)
    {
        MapShaderResources();
        m_isDescriptorSetLayoutsDirty = false;
    }

    const auto cmd = currentFrame->CommandBuffer();
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    SetViewportAndScissorFullWindow(cmd, dimensions);

    for (auto& shadowMappingPass : m_shadowMappingPasses)
    {
        shadowMappingPass.Begin(cmd, frameBufferIndex);
        shadowMappingPass.Execute(cmd, frameData, frameIndex);
        shadowMappingPass.End(cmd);
    }

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    m_litPass.Begin(cmd, frameBufferIndex);
    m_litPass.Execute(cmd, frameData, frameIndex);
    m_litPass.End(cmd);
    cmd->end();
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    m_litPass = CreateLitPass(m_device, m_gpuAllocator, m_swapchain, dimensions);

    m_shadowMappingPasses.clear();
    for (auto i : std::views::iota(0u, m_maxLights))
    {
        m_shadowMappingPasses.push_back(std::move(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i)));
    }
    m_isDescriptorSetLayoutsDirty = true;
}

void RenderGraph::IncrementShadowPassCount()
{
    NC_ASSERT(m_activeShadowMappingPasses < m_maxLights, "Tried to add a light source when max lights are registered.");
    m_activeShadowMappingPasses++;
    m_isDescriptorSetLayoutsDirty = true;
}

void RenderGraph::ClearShadowPasses()
{
    m_activeShadowMappingPasses = 0u;
    m_isDescriptorSetLayoutsDirty = true;
}

void RenderGraph::DecrementShadowPassCount()
{
    NC_ASSERT(m_activeShadowMappingPasses > 0, "Tried to remove a light source when none are registered.");
    m_activeShadowMappingPasses--;
    m_isDescriptorSetLayoutsDirty = true;
}
} // namespace nc::graphics
