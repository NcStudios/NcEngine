#include "RenderGraph.h"
#include "graphics/GraphicsUtilities.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderStorage.h"
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

auto CreateShadowMappingPass(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain* swapchain, nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager, const nc::Vector2& dimensions, uint32_t shadowCasterIndex, bool isOmniDirectional) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

    const auto vkDevice = device->VkDevice();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    auto attachment = std::vector<Attachment>{};
    attachment.push_back(Attachment(vkDevice, allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachment), size, ClearValueFlags::Depth);

    const auto views = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(0u)};
    renderPass.CreateFrameBuffers(views, dimensions);
    renderPass.RegisterShadowMappingTechnique(vkDevice, shaderBindingManager, shadowCasterIndex, isOmniDirectional);

    return renderPass;
}

auto CreateLitPass(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain* swapchain, nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager, uint32_t swapchainImageIndex, const nc::Vector2& dimensions) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

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
    auto renderPass = RenderPass(vkDevice, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto colorResolveView = renderPass.GetAttachmentView(1);
    auto depthImageView   = renderPass.GetAttachmentView(0);
    auto colorImageView   = swapchain->GetColorImageViews().at(swapchainImageIndex).get();

    auto imageViews = std::array<vk::ImageView, 3>
    {
        colorResolveView, // Color Resolve View
        depthImageView, // Depth View
        colorImageView // Swapchain Image at index <swapchainImageIndex>
    };

    renderPass.CreateFrameBuffers(imageViews, dimensions);

    // Create all the lit pass pipelines
    #ifdef NC_EDITOR_ENABLED
    renderPass.RegisterTechnique<WireframeTechnique>(device, shaderBindingManager);
    #endif

    renderPass.RegisterTechnique<EnvironmentTechnique>(device, shaderBindingManager);
    renderPass.RegisterTechnique<PbrTechnique>(device, shaderBindingManager);
    renderPass.RegisterTechnique<ToonTechnique>(device, shaderBindingManager);
    renderPass.RegisterTechnique<OutlineTechnique>(device, shaderBindingManager);
    renderPass.RegisterTechnique<ParticleTechnique>(device, shaderBindingManager);
    renderPass.RegisterTechnique<UiTechnique>(device, shaderBindingManager);

    return renderPass;
}

auto CreatePerFrameGraphs(const nc::graphics::vulkan::Device* device,
                        nc::graphics::vulkan::Swapchain* swapchain,
                        nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager,
                        nc::graphics::vulkan::GpuAllocator* gpuAllocator,
                        nc::Vector2 dimensions)
{
    return [&] <size_t... N> (std::index_sequence<N...>)
    {
        return std::array{((void)N, nc::graphics::vulkan::PerFrameRenderGraph(device, swapchain, shaderBindingManager, gpuAllocator, dimensions, N))...};
    }(std::make_index_sequence<nc::graphics::MaxFramesInFlight>());
}
}

namespace nc::graphics::vulkan
{
RenderGraph::RenderGraph(FrameManager* frameManager, Registry* registry, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, Vector2 dimensions, uint32_t maxLights)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shaderStorage{shaderStorage},
      m_dummyShadowMap{Attachment(m_device->VkDevice(), m_gpuAllocator, Vector2{1.0f, 1.0f}, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm)},
      m_perFrameRenderGraphs{CreatePerFrameGraphs(device, swapchain, shaderBindingManager, gpuAllocator, dimensions)},
      m_onDescriptorSetsChanged{m_shaderBindingManager->OnResourceLayoutChanged().Connect(this, &RenderGraph::SetDescriptorSetLayoutsDirty)},
      m_onCommitOmniLight{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){IncrementShadowPassCount(true);})},
      m_onRemoveOmniLight{registry->OnRemove<PointLight>().Connect([this](Entity){DecrementShadowPassCount(true);})},
      m_onCommitUniLight{registry->OnCommit<SpotLight>().Connect([this](graphics::SpotLight&){IncrementShadowPassCount(false);})},
      m_onRemoveUniLight{registry->OnRemove<SpotLight>().Connect([this](Entity){DecrementShadowPassCount(false);})},
      m_dimensions{dimensions},
      m_screenExtent{},
      m_omniDirLightCount{},
      m_uniDirLightCount{},
      m_maxLights{maxLights}
{
}

void RenderGraph::SinkPostProcessImages()
{
    OPTICK_CATEGORY("RenderGraph::SinkPostProcessImages", Optick::Category::Rendering);

    auto frameIndex = m_frameManager->CurrentFrameContext()->Index();
    auto& renderGraph = m_perFrameRenderGraphs.at(frameIndex);

    // Sink shadow maps from the render target of each shadow pass into a vector of post process images.
    auto shadowMapsSink = std::vector<vk::ImageView>{};
    shadowMapsSink.reserve(renderGraph.shadowPasses.size());

    std::ranges::transform(renderGraph.shadowPasses, std::back_inserter(shadowMapsSink), [](auto& shadowPass)
    {
        return shadowPass.GetAttachmentView(0u);
    });

    m_shaderStorage->SinkPostProcessImages(shadowMapsSink, PostProcessImageType::ShadowMap, frameIndex);
}

void RenderGraph::CommitResourceLayout()
{
    auto& renderGraph = m_perFrameRenderGraphs.at(m_frameManager->CurrentFrameContext()->Index());
    if (!renderGraph.isDirty)
    {
        return;
    }

    renderGraph.shadowPasses.clear();
    for (auto i : std::views::iota(0u, m_omniDirLightCount))
    {
        renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, true));
    }

    for (auto i : std::views::iota(m_omniDirLightCount, m_omniDirLightCount + m_uniDirLightCount))
    {
        renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, false));
    }

    renderGraph.isDirty = false;
}

void RenderGraph::RecordDrawCallsOnBuffer(const PerFrameRenderState &frameData, const Vector2& dimensions, const Vector2& screenExtent)
{
    OPTICK_CATEGORY("RenderGraph::RecordDrawCallsOnBuffer", Optick::Category::Rendering);

    auto frameIndex = m_frameManager->CurrentFrameContext()->Index();
    auto& renderGraph = m_perFrameRenderGraphs.at(frameIndex);
    const auto cmd = m_frameManager->CurrentFrameContext()->CommandBuffer();

    SetViewportAndScissorFullWindow(cmd, dimensions);

    for (auto& shadowMappingPass : renderGraph.shadowPasses)
    {
        shadowMappingPass.Begin(cmd);
        shadowMappingPass.Execute(cmd, frameData, frameIndex);
        shadowMappingPass.End(cmd);
    }

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    renderGraph.litPass.Begin(cmd);
    renderGraph.litPass.Execute(cmd, frameData, frameIndex);
    renderGraph.litPass.End(cmd);
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    m_perFrameRenderGraphs = CreatePerFrameGraphs(m_device, m_swapchain, m_shaderBindingManager, m_gpuAllocator, dimensions);

    for (auto& renderGraph : m_perFrameRenderGraphs)
    {
        renderGraph.shadowPasses.clear();
        for (auto i : std::views::iota(0u, m_omniDirLightCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, dimensions, i, true));
        }

        for (auto i : std::views::iota(m_omniDirLightCount, m_omniDirLightCount + m_uniDirLightCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, dimensions, i, false));
        }
    }
}

void RenderGraph::IncrementShadowPassCount(bool isOmniDirectional)
{
    NC_ASSERT(m_omniDirLightCount + m_uniDirLightCount < m_maxLights, "Tried to add a light source when max lights are registered.");

    if (isOmniDirectional)
    {
        m_omniDirLightCount++;
    }
    else
    {
        m_uniDirLightCount++;
    }
    
    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::ClearShadowPasses() noexcept
{
    m_omniDirLightCount = 0u;
    m_uniDirLightCount = 0u;
    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::DecrementShadowPassCount(bool isOmniDirectional)
{
    if (isOmniDirectional)
    {
        NC_ASSERT(m_omniDirLightCount > 0, "Tried to remove a light source when none are registered.");
        m_omniDirLightCount--;
    }
    else
    {
        NC_ASSERT(m_uniDirLightCount > 0, "Tried to remove a light source when none are registered.");
        m_uniDirLightCount--;
    }

    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&)
{
    for (auto& renderGraph : m_perFrameRenderGraphs)
    {
        renderGraph.isDirty = true;
    }
}

PerFrameRenderGraph::PerFrameRenderGraph(const Device* device,
                                         Swapchain* swapchain,
                                         ShaderBindingManager* shaderBindingManager,
                                         GpuAllocator* gpuAllocator,
                                         Vector2 dimensions,
                                         uint32_t index)
    : shadowPasses{},
      litPass{CreateLitPass(device, gpuAllocator, swapchain, shaderBindingManager, index, dimensions)},
      isDirty{false}
{
}
} // namespace nc::graphics::vulkan
