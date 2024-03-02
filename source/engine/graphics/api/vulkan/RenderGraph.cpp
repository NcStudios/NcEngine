#include "RenderGraph.h"
#include "graphics/GraphicsUtilities.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/techniques/EnvironmentTechnique.h"
#include "graphics/api/vulkan/techniques/OutlineTechnique.h"
#include "graphics/api/vulkan/techniques/ParticleTechnique.h"
#include "graphics/api/vulkan/techniques/PbrTechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"
#include "graphics/api/vulkan/techniques/ToonTechnique.h"
#include "graphics/api/vulkan/techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/api/vulkan/techniques/WireframeTechnique.h"
#endif

#include "optick.h"

#include <array>
#include <ranges>
#include <string>

namespace
{
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

auto CreateShadowMappingPass(const nc::graphics::Device* device, nc::graphics::GpuAllocator* allocator, nc::graphics::Swapchain* swapchain, const nc::Vector2& dimensions, uint32_t index) -> std::unique_ptr<nc::graphics::RenderPass>
{
    using namespace nc::graphics;

    const auto vkDevice = device->VkDevice();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    std::vector<Attachment> attachments;
    const auto numConcurrentAttachments = MaxFramesInFlight;
    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
    }

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = std::make_unique<RenderPass>(vkDevice, ShadowMappingPassId + std::to_string(index), shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth);

    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        const auto views = std::array<vk::ImageView, 1>{renderPass->GetAttachmentView(i)};
        renderPass->RegisterAttachmentViews(views, dimensions, i);
    }

    return renderPass;
}

auto CreateLitPass(const nc::graphics::Device* device, nc::graphics::GpuAllocator* allocator, nc::graphics::Swapchain* swapchain, const nc::Vector2& dimensions) -> std::unique_ptr<nc::graphics::RenderPass>
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
    auto renderPass = std::make_unique<RenderPass>(vkDevice, LitPassId, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto &colorImageViews = swapchain->GetColorImageViews();
    auto depthImageView = renderPass->GetAttachmentView(0);
    auto colorResolveView = renderPass->GetAttachmentView(1);

    uint32_t index = 0;
    for (auto &imageView : colorImageViews)
    {
        std::vector<vk::ImageView> imageViews
        {
            colorResolveView, // Color Resolve View
            depthImageView, // Depth View
            imageView.get()
        };
        renderPass->RegisterAttachmentViews(imageViews, dimensions, index++);
    }
    return renderPass;
}
}

namespace nc::graphics
{
RenderGraph::RenderGraph(FrameManager* frameManager, Registry* registry, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, Vector2 dimensions, uint32_t maxLights)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shadowMappingPasses{},
      m_litPass{CreateLitPass(device, m_gpuAllocator, m_swapchain, dimensions)},
      m_postProcessImageViews{},
      m_dummyShadowMap{Attachment(m_device->VkDevice(), m_gpuAllocator, Vector2{1.0f, 1.0f}, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm)},
      m_onDescriptorSetsChanged{m_shaderBindingManager->OnResourceLayoutChanged().Connect(this, &RenderGraph::SetDescriptorSetLayoutsDirty)},
      m_onCommitPointLightConnection{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){IncrementShadowPassCount();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){DecrementShadowPassCount();})},
      m_dimensions{dimensions},
      m_screenExtent{},
      m_activeShadowMappingPasses{},
      m_maxLights{maxLights},
      m_isDescriptorSetLayoutsDirty{std::array<bool, MaxFramesInFlight>{true, true}}
{
    auto view = m_dummyShadowMap.view.get();
    m_postProcessImageViews.emplace(PostProcessImageType::ShadowMap, PostProcessViews
    {
        .perFrameViews = std::array<std::vector<vk::ImageView>, MaxFramesInFlight>
        {
            std::vector<vk::ImageView>(m_maxLights, view),
            std::vector<vk::ImageView>(m_maxLights, view)
        }
    });
}

void RenderGraph::SinkPostProcessImages()
{
    OPTICK_CATEGORY("RenderGraph::SinkPostProcessImages", Optick::Category::Rendering);

    auto* currentFrame = m_frameManager->CurrentFrameContext();
    auto view = m_dummyShadowMap.view.get();
    m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews.at(currentFrame->Index()) = std::vector<vk::ImageView>(m_maxLights, view);

    for (auto i : std::views::iota(0u, m_activeShadowMappingPasses))
    {
        m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews.at(currentFrame->Index()).at(i) = m_shadowMappingPasses.at(i)->GetAttachmentView(currentFrame->Index());
    }
}

auto RenderGraph::GetPostProcessImages(PostProcessImageType imageType) -> std::vector<vk::ImageView>
{
    auto* currentFrame = m_frameManager->CurrentFrameContext();
    return m_postProcessImageViews.at(imageType).perFrameViews.at(currentFrame->Index());
}

void RenderGraph::CommitResourceLayout()
{
    // Wait for submission of the command buffer on the queue to be complete.
    m_device->VkGraphicsQueue().waitIdle();

    auto* currentFrame = m_frameManager->CurrentFrameContext();

    if (!m_isDescriptorSetLayoutsDirty.at(currentFrame->Index()))
    {
        return;
    }

    m_shadowMappingPasses.clear();

    for (auto i : std::views::iota(0u, m_activeShadowMappingPasses))
    {
        m_shadowMappingPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i));
        m_shadowMappingPasses[i]->ClearTechniques();
        m_shadowMappingPasses[i]->RegisterShadowMappingTechnique(m_device->VkDevice(), m_shaderBindingManager, i);
    }

    m_litPass->ClearTechniques();

    #ifdef NC_EDITOR_ENABLED
    m_litPass->RegisterTechnique<WireframeTechnique>(*m_device, m_shaderBindingManager);
    #endif

    m_litPass->RegisterTechnique<EnvironmentTechnique>(*m_device, m_shaderBindingManager);
    m_litPass->RegisterTechnique<PbrTechnique>(*m_device, m_shaderBindingManager);
    m_litPass->RegisterTechnique<ToonTechnique>(*m_device, m_shaderBindingManager);
    m_litPass->RegisterTechnique<OutlineTechnique>(*m_device, m_shaderBindingManager);
    m_litPass->RegisterTechnique<ParticleTechnique>(*m_device, m_shaderBindingManager);
    m_litPass->RegisterTechnique<UiTechnique>(*m_device, m_shaderBindingManager);
    m_isDescriptorSetLayoutsDirty.at(currentFrame->Index()) = false;
}

void RenderGraph::RecordDrawCallsOnBuffer(const PerFrameRenderState &frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent)
{
    OPTICK_CATEGORY("RenderGraph::RecordDrawCallsOnBuffer", Optick::Category::Rendering);

    auto* currentFrame = m_frameManager->CurrentFrameContext();
    const auto cmd = currentFrame->CommandBuffer();

    SetViewportAndScissorFullWindow(cmd, dimensions);

    for (auto& shadowMappingPass : m_shadowMappingPasses)
    {
        shadowMappingPass->Begin(cmd, frameBufferIndex);
        shadowMappingPass->Execute(cmd, frameData, currentFrame->Index());
        shadowMappingPass->End(cmd);
    }

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    m_litPass->Begin(cmd, frameBufferIndex);
    m_litPass->Execute(cmd, frameData, currentFrame->Index());
    m_litPass->End(cmd);
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    m_litPass = CreateLitPass(m_device, m_gpuAllocator, m_swapchain, dimensions);

    m_shadowMappingPasses.clear();
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews.at(i).clear();
    }

    for (auto i : std::views::iota(0u, m_activeShadowMappingPasses))
    {
        m_shadowMappingPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i));

        for (auto j : std::views::iota(0u, MaxFramesInFlight))
        {
            m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews.at(j).emplace_back(m_shadowMappingPasses.back()->GetAttachmentView(j));
        }
    }

    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_isDescriptorSetLayoutsDirty.at(i) = true;
    }
}

void RenderGraph::IncrementShadowPassCount()
{
    NC_ASSERT(m_activeShadowMappingPasses < m_maxLights, "Tried to add a light source when max lights are registered.");
    m_activeShadowMappingPasses++;
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_isDescriptorSetLayoutsDirty.at(i) = true;
    }
}

void RenderGraph::ClearShadowPasses()
{
    m_activeShadowMappingPasses = 0u;
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_isDescriptorSetLayoutsDirty.at(i) = true;
    }
}

void RenderGraph::DecrementShadowPassCount()
{
    NC_ASSERT(m_activeShadowMappingPasses > 0, "Tried to remove a light source when none are registered.");
    m_activeShadowMappingPasses--;
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_isDescriptorSetLayoutsDirty.at(i) = true;
    }
}

void RenderGraph::SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&)
{
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_isDescriptorSetLayoutsDirty.at(i) = true;
    }
}
} // namespace nc::graphics
