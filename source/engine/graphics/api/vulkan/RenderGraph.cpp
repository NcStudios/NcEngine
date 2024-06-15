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

auto CreateLitPass(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain* swapchain, const nc::Vector2& dimensions) -> nc::graphics::vulkan::RenderPass
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

    auto colorResolveView    = renderPass.GetAttachmentView(1);
    auto depthImageView      = renderPass.GetAttachmentView(0);
    auto& swapchainImageViews = swapchain->GetColorImageViews();

    for (auto& swapchainImageView : swapchainImageViews)
    {
        auto imageViews = std::array<vk::ImageView, 3>
        {
            colorResolveView, // Color Resolve View
            depthImageView, // Depth View
            swapchainImageView.get() // Swapchain Image at index <swapchainImageIndex>
        };

        renderPass.CreateFrameBuffers(imageViews, dimensions);
    }

    return renderPass;
}

auto CreatePerFrameGraphs(const nc::graphics::vulkan::Device* device,
                        nc::graphics::vulkan::Swapchain* swapchain,
                        nc::graphics::vulkan::GpuAllocator* gpuAllocator,
                        nc::Vector2 dimensions)
{
    return [&] <size_t... N> (std::index_sequence<N...>)
    {
        return std::array{((void)N, nc::graphics::vulkan::PerFrameRenderGraph(device, swapchain, gpuAllocator, dimensions))...};
    }(std::make_index_sequence<nc::graphics::MaxFramesInFlight>());
}
}

namespace nc::graphics::vulkan
{
RenderGraph::RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, Vector2 dimensions, uint32_t maxLights)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shaderStorage{shaderStorage},
      m_dummyShadowMap{Attachment(m_device->VkDevice(), m_gpuAllocator, Vector2{1.0f, 1.0f}, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm)},
      m_perFrameRenderGraphs{CreatePerFrameGraphs(device, swapchain, gpuAllocator, dimensions)},
      m_dimensions{dimensions},
      m_screenExtent{},
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

void RenderGraph::BuildRenderGraph(PerFrameRenderStateData stateData, uint32_t frameIndex)
{
    auto& renderGraph = m_perFrameRenderGraphs.at(frameIndex);

    // If lights have been added or removed we need to add or remove shadow passes (one per light)
    if (stateData.omniDirLightsCount != renderGraph.stateData.omniDirLightsCount || 
        stateData.uniDirLightsCount  != renderGraph.stateData.uniDirLightsCount)
    {
        renderGraph.shadowPasses.clear();
        for (auto i : std::views::iota(0u, stateData.omniDirLightsCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, true));
        }

        // Note: These are currently identical loops but will be split out when the pass details change for omnidirectional shadow maps.
        for (auto i : std::views::iota(stateData.omniDirLightsCount, stateData.omniDirLightsCount + stateData.uniDirLightsCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, false));
        }
    }

    // All other state data pertains to the lit pass. If any of the comparisons have changed, we'll need to register the appropriate pipelines
    if (stateData != renderGraph.stateData)
    {
        renderGraph.litPass.UnregisterPipelines();

        #ifdef NC_EDITOR_ENABLED
        if (stateData.widgetsCount)
            renderGraph.litPass.RegisterPipeline<WireframeTechnique>(m_device, m_shaderBindingManager);
        #endif

        if (stateData.useSkybox)
            renderGraph.litPass.RegisterPipeline<EnvironmentTechnique>(m_device, m_shaderBindingManager);

        if (stateData.meshRenderersCount)
            renderGraph.litPass.RegisterPipeline<PbrTechnique>(m_device, m_shaderBindingManager);

        if (stateData.toonRenderersCount)
        {
            renderGraph.litPass.RegisterPipeline<ToonTechnique>(m_device, m_shaderBindingManager);
            renderGraph.litPass.RegisterPipeline<OutlineTechnique>(m_device, m_shaderBindingManager);
        }

        if (stateData.particlesCount)
            renderGraph.litPass.RegisterPipeline<ParticleTechnique>(m_device, m_shaderBindingManager);

        renderGraph.litPass.RegisterPipeline<UiTechnique>(m_device, m_shaderBindingManager);
    }
    renderGraph.stateData = stateData;
}

void RenderGraph::RecordDrawCallsOnBuffer(const PerFrameRenderState &frameData, const Vector2& dimensions, const Vector2& screenExtent, uint32_t swapchainImageIndex)
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

    renderGraph.litPass.Begin(cmd, swapchainImageIndex);
    renderGraph.litPass.Execute(cmd, frameData, frameIndex);
    renderGraph.litPass.End(cmd);
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    auto renderStateCaches = std::vector<PerFrameRenderStateData>{};
    renderStateCaches.reserve(m_perFrameRenderGraphs.size());

    for (auto& renderGraph : m_perFrameRenderGraphs)
    {
        renderStateCaches.push_back(renderGraph.stateData);
    }

    m_perFrameRenderGraphs = CreatePerFrameGraphs(m_device, m_swapchain, m_gpuAllocator, dimensions);
    for (auto i  : std::views::iota(0u, renderStateCaches.size()))
    {
        BuildRenderGraph(renderStateCaches.at(i), i);
    }
}

PerFrameRenderGraph::PerFrameRenderGraph(const Device* device,
                                         Swapchain* swapchain,
                                         GpuAllocator* gpuAllocator,
                                         Vector2 dimensions)
    : shadowPasses{},
      litPass{CreateLitPass(device, gpuAllocator, swapchain, dimensions)}
{
}
} // namespace nc::graphics::vulkan
