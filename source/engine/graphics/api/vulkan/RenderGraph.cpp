#include "RenderGraph.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderStorage.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/techniques/EnvironmentTechnique.h"
#include "graphics/api/vulkan/techniques/OutlineTechnique.h"
#include "graphics/api/vulkan/techniques/ParticleTechnique.h"
#include "graphics/api/vulkan/techniques/PbrTechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"
#include "graphics/api/vulkan/techniques/ToonTechnique.h"
#include "graphics/api/vulkan/techniques/UiTechnique.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "graphics/shader_resource/ShaderResourceBus.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/api/vulkan/techniques/WireframeTechnique.h"
#endif

#include "optick.h"

#include <array>
#include <ranges>

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

// auto CreateShadowMappingPassOmni(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain*, nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager, const nc::Vector2& , uint32_t shadowCasterIndex, bool isOmniDirectional) -> nc::graphics::vulkan::RenderPass
// {
//     using namespace nc::graphics::vulkan;

//     const auto vkDevice = device->VkDevice();
//     const auto& gpuOptions = device->GetGpuOptions();
//     const auto shadowAttachmentSlots = std::array<AttachmentSlot, 2>
//     {
//         AttachmentSlot // Color blit source
//         {
//             0,
//             vk::Format::eR32Sfloat,                   // Image format
//             vk::ImageLayout::eShaderReadOnlyOptimal,  // Initial layout
//             vk::ImageLayout::eColorAttachmentOptimal, // Initial subpass layout
//             vk::ImageLayout::eShaderReadOnlyOptimal,  // Final layout
//             vk::AttachmentLoadOp::eClear,             // Attachment load
//             vk::AttachmentStoreOp::eStore,            // Attachment store
//             vk::AttachmentLoadOp::eDontCare,          // Stencil load
//             vk::AttachmentStoreOp::eDontCare,         // Stencil store
//             vk::SampleCountFlagBits::e1               // Sample count
//         },
//         AttachmentSlot // Depth image
//         {
//             1,
//             gpuOptions.GetDepthFormat(),                     // Image format
//             vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial layout
//             vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial subpass layout
//             vk::ImageLayout::eDepthStencilAttachmentOptimal, // Final layout
//             vk::AttachmentLoadOp::eClear,                    // Attachment load
//             vk::AttachmentStoreOp::eStore,                   // Attachment store
//             vk::AttachmentLoadOp::eDontCare,                 // Stencil load
//             vk::AttachmentStoreOp::eDontCare,                // Stencil store
//             vk::SampleCountFlagBits::e1                      // Sample count
//         }
//     };

//     auto shadowMapResolution = nc::Vector2{1024, 1024};
//     auto shadowMapExtent = vk::Extent2D{1024, 1024};

//     const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots.at(0), shadowAttachmentSlots.at(1)}};

//     auto attachments = std::vector<Attachment>{};
//     attachments.push_back(Attachment(vkDevice, allocator, shadowMapResolution, false, shadowAttachmentSlots.at(0).numSamples, shadowAttachmentSlots.at(0).format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc));
//     attachments.push_back(Attachment(vkDevice, allocator, shadowMapResolution, true, shadowAttachmentSlots.at(1).numSamples, shadowAttachmentSlots.at(1).format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));

//     const auto size = AttachmentSize{shadowMapResolution, shadowMapExtent};
//     auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

//     auto attachmentViews = std::array<vk::ImageView, 2>{};
//     attachmentViews.at(1) = renderPass.GetRenderTarget(1);

//     for
//     renderPass.CreateFrameBuffer(views, shadowMapResolution); // Create a cube map PPIA with six views. Get them here
//     renderPass.RegisterShadowMappingTechnique(vkDevice, shaderBindingManager, shadowCasterIndex, isOmniDirectional);

//     return renderPass;
// }

auto CreateShadowMappingPass(const nc::graphics::vulkan::Device* device,
                             nc::graphics::vulkan::GpuAllocator* allocator,
                             nc::graphics::vulkan::Swapchain* swapchain,
                             nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager,
                             const nc::Vector2& dimensions,
                             uint32_t shadowCasterIndex,
                             bool isOmniDirectional) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

    const auto vkDevice = device->VkDevice();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot
        {
            0,
            vk::Format::eD16Unorm,                                   // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Initial subpass layout
            vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal, // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            vk::SampleCountFlagBits::e1                              // Sample count
        }
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    auto attachments = std::vector<Attachment>{};
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, shadowAttachmentSlots[0].numSamples, shadowAttachmentSlots[0].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled));

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderTargets = std::vector<vk::ImageView>{attachments.at(0).view.get()};
    auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth, nc::graphics::PostProcessImageType::ShadowMap, std::move(renderTargets), 0u);

    renderPass.CreateFrameBuffer(renderTargets, dimensions);
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
        AttachmentSlot // Swapchain Image (Color)
        {
            0,
            swapchain->GetFormat(),                                  // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Initial subpass layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            numSamples                                               // Sample count
        },
        AttachmentSlot // Depth
        {
            1,
            gpuOptions.GetDepthFormat(),                             // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Initial subpass layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eDontCare,                        // Attachment store
            vk::AttachmentLoadOp::eClear,                            // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            numSamples                                               // Sample count
        },
        AttachmentSlot // AA Color Resolve
        {
            2,
            swapchain->GetFormat(),                                  // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Initial subpass layout
            vk::ImageLayout::ePresentSrcKHR,                         // Final layout
            vk::AttachmentLoadOp::eDontCare,                         // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            vk::SampleCountFlagBits::e1                              // Sample count
        },
    };

    const auto litSubpasses = std::array<Subpass, 1>
    {
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
    };

    std::vector<Attachment> attachments;
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, litAttachmentSlots[1].numSamples, litAttachmentSlots[1].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled)); // Depth Stencil
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, false, litAttachmentSlots[0].numSamples, litAttachmentSlots[0].format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment)); // AA Color Resolve Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto swapchainImageViews = swapchain->GetSwapchainImageViews();
    for (auto& swapchainImageView : swapchainImageViews) // We need a framebuffer with all three views (depth, AA resolve, swapchain) per swapchain image view.
    {
        auto imageViews = std::array<vk::ImageView, 3>
        {
            attachments.at(1).view.get(), // Color Resolve View
            attachments.at(0).view.get(), // Depth View
            swapchainImageView // Swapchain Image at index <swapchainImageIndex>
        };

        renderPass.CreateFrameBuffer(imageViews, dimensions);
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
RenderGraph::RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, ShaderResourceBus& shaderResourceBus, Vector2 dimensions)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shaderStorage{shaderStorage},
      m_perFrameRenderGraphs{CreatePerFrameGraphs(m_device, m_swapchain, m_gpuAllocator, dimensions)},
      m_renderTargetsBuffers{{PostProcessImageType::ShadowMap, shaderResourceBus->CreatePPImageArrayBuffer(PostProcessImageType::ShadowMap, 20u, ShaderStage::Fragment, 3u, 0u)}},
      m_dimensions{dimensions},
      m_screenExtent{}
{
}

// Sink outputs from the render graph into shader storage
void RenderGraph::SinkRenderTargets(const RenderPass& renderPass)
{
    OPTICK_CATEGORY("RenderGraph::SinkRenderTargets", Optick::Category::Rendering);
    auto& renderGraph = GetCurrentFrameGraph();
    auto renderTargetsType = renderPass.GetRenderTargetsType();
    auto& renderTargetsBuffer = m_renderTargetsBuffer.at(renderTargetsType);
    m_shaderStorage->SinkRenderTargets(renderPass.GetRenderTargets(), renderTargetsType, m_frameManager->Index());

    if (renderGraph.updateRenderTargets.at(renderTargetsType))
    {
        renderTargetsBuffer.Update(m_frameManager->Index());
    }
}

void RenderGraph::BuildRenderGraph(const PerFrameRenderStateData& stateData, uint32_t frameIndex)
{
    OPTICK_CATEGORY("RenderGraph::BuildRenderGraph", Optick::Category::Rendering);
    auto& renderGraph = m_perFrameRenderGraphs.at(frameIndex);

    // If lights have been added or removed we need to add or remove shadow passes (one per light)
    if (stateData.omniDirLightsCount != renderGraph.stateData.omniDirLightsCount || 
        stateData.uniDirLightsCount  != renderGraph.stateData.uniDirLightsCount)
    {
        renderGraph.updateRenderTargets.at(PostProcessImageType::ShadowMap) = true;

        renderGraph.shadowPasses.clear();
        for (auto i : std::views::iota(0u, stateData.omniDirLightsCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, true));
        }

        for (auto i : std::views::iota(stateData.omniDirLightsCount, stateData.omniDirLightsCount + stateData.uniDirLightsCount))
        {
            renderGraph.shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_shaderBindingManager, m_dimensions, i, false));
        }
    }

    // All other state data pertains to the lit pass. If any of the comparisons have changed, we'll need to register the appropriate pipelines
    if (stateData != renderGraph.stateData || !renderGraph.isInitialized)
    {
        renderGraph.litPass.UnregisterPipelines();

        #ifdef NC_EDITOR_ENABLED
        if (!renderGraph.isInitialized || stateData.widgetsCount)
            renderGraph.litPass.RegisterPipeline<WireframeTechnique>(m_device, m_shaderBindingManager);
        #endif

        if (!renderGraph.isInitialized || stateData.useSkybox)
            renderGraph.litPass.RegisterPipeline<EnvironmentTechnique>(m_device, m_shaderBindingManager);

        if (!renderGraph.isInitialized || stateData.meshRenderersCount)
            renderGraph.litPass.RegisterPipeline<PbrTechnique>(m_device, m_shaderBindingManager);

        if (!renderGraph.isInitialized || stateData.toonRenderersCount)
        {
            renderGraph.litPass.RegisterPipeline<ToonTechnique>(m_device, m_shaderBindingManager);
            renderGraph.litPass.RegisterPipeline<OutlineTechnique>(m_device, m_shaderBindingManager);
        }

        if (!renderGraph.isInitialized || stateData.particlesCount)
            renderGraph.litPass.RegisterPipeline<ParticleTechnique>(m_device, m_shaderBindingManager);

        renderGraph.litPass.RegisterPipeline<UiTechnique>(m_device, m_shaderBindingManager);
    }

    renderGraph.stateData = stateData;
    renderGraph.isInitialized = true;
}

void RenderGraph::RecordDrawCallsOnBuffer(const PerFrameRenderState &frameData, const Vector2& dimensions, const Vector2& screenExtent, uint32_t swapchainImageIndex)
{
    OPTICK_CATEGORY("RenderGraph::RecordDrawCallsOnBuffer", Optick::Category::Rendering);

    auto frame = m_frameManager->CurrentFrameContext();
    const auto cmd = frame->CommandBuffer();
    auto frameIndex = frame->Index();
    auto& renderGraph = GetCurrentFrameGraph();

    SetViewportAndScissorFullWindow(cmd, dimensions);

    for (auto& shadowMappingPass : renderGraph.shadowPasses)
    {
        shadowMappingPass.Begin(cmd);
        shadowMappingPass.Execute(cmd, frameData, frameIndex);
        shadowMappingPass.End(cmd);
        SinkRenderTargets(shadowMappingPass); // Call shaderstorage sink render targets. call bufferHandle.Update if isDirty
    }
    renderGraph.updateRenderTargets.at(PostProcessImageType::ShadowMap) = false;

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    renderGraph.litPass.Begin(cmd, swapchainImageIndex);
    renderGraph.litPass.Execute(cmd, frameData, frameIndex);
    renderGraph.litPass.End(cmd);
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    Clear();
    auto renderStateCaches = std::vector<PerFrameRenderStateData>{};
    renderStateCaches.reserve(m_perFrameRenderGraphs.size());

    for (auto& renderGraph : m_perFrameRenderGraphs)
    {
        renderStateCaches.push_back(renderGraph.stateData);
    }

    m_perFrameRenderGraphs = CreatePerFrameGraphs(m_device, m_swapchain, m_gpuAllocator, dimensions);
    for (const auto& [index, cache] : std::views::enumerate(renderStateCaches))
    {
        BuildRenderGraph(cache, static_cast<uint32_t>(index));
    }
}

void RenderGraph::Clear()
{
    for (auto& renderGraph : m_perFrameRenderGraphs)
    {
        renderGraph.litPass.UnregisterPipelines();
        renderGraph.isInitialized = false;
    }
}

PerFrameRenderGraph::PerFrameRenderGraph(const Device* device,
                                         Swapchain* swapchain,
                                         GpuAllocator* gpuAllocator,
                                         Vector2 dimensions)
    : shadowPasses{},
      litPass{CreateLitPass(device, gpuAllocator, swapchain, dimensions)},
      updateRenderTargets{{PostProcessImageType::ShadowMap, false}},
      isInitialized{false}
{
}
} // namespace nc::graphics::vulkan
