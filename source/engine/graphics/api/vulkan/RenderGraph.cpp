#include "RenderGraph.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderStorage.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/pipelines/EnvironmentPipeline.h"
#include "graphics/api/vulkan/pipelines/OutlinePipeline.h"
#include "graphics/api/vulkan/pipelines/ParticlePipeline.h"
#include "graphics/api/vulkan/pipelines/PbrPipeline.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingPipeline.h"
#include "graphics/api/vulkan/pipelines/ToonPipeline.h"
#include "graphics/api/vulkan/pipelines/UiPipeline.h"
#include "graphics/shader_resource/RenderPassSinkBufferHandle.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "ncengine/asset/AssetData.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/api/vulkan/pipelines/WireframePipeline.h"
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

auto CreateShadowMappingPassOmni(const nc::graphics::vulkan::Device* device,
                                 nc::graphics::vulkan::GpuAllocator* allocator,
                                 nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager,
                                 std::span<const vk::ImageView> sourceViews) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

    const auto vkDevice = device->VkDevice();
    const auto& gpuOptions = device->GetGpuOptions();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 2>
    {
        AttachmentSlot // Color blit source
        {
            0,
            vk::Format::eR32Sfloat,                   // Image format
            vk::ImageLayout::eShaderReadOnlyOptimal,  // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal, // Initial subpass layout
            vk::ImageLayout::eShaderReadOnlyOptimal,  // Final layout
            vk::AttachmentLoadOp::eClear,             // Attachment load
            vk::AttachmentStoreOp::eStore,            // Attachment store
            vk::AttachmentLoadOp::eDontCare,          // Stencil load
            vk::AttachmentStoreOp::eDontCare,         // Stencil store
            vk::SampleCountFlagBits::e1               // Sample count
        },
        AttachmentSlot // Depth image
        {
            1,
            gpuOptions.GetDepthFormat(),                     // Image format
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial subpass layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Final layout
            vk::AttachmentLoadOp::eClear,                    // Attachment load
            vk::AttachmentStoreOp::eStore,                   // Attachment store
            vk::AttachmentLoadOp::eDontCare,                 // Stencil load
            vk::AttachmentStoreOp::eDontCare,                // Stencil store
            vk::SampleCountFlagBits::e1                      // Sample count
        }
    };

    auto shadowMapExtent = vk::Extent2D{static_cast<uint32_t>(ShadowMapDimensions.x), static_cast<uint32_t>(ShadowMapDimensions.y)};

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots.at(0), shadowAttachmentSlots.at(1)}};

    auto attachments = std::vector<Attachment>{};
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, false, shadowAttachmentSlots.at(0).numSamples, shadowAttachmentSlots.at(0).format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc)); // Attachment to be blitted to a cube face
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, shadowAttachmentSlots.at(1).numSamples, shadowAttachmentSlots.at(1).format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));

    const auto size = AttachmentSize{ShadowMapDimensions, shadowMapExtent};
    auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto attachmentViews = std::array<vk::ImageView, 2>{};
    attachmentViews.at(1) = renderPass.GetAttachmentView(1u);

    for (auto cubeMapFaceView : sourceViews)
    {
        attachmentViews.at(0) = cubeMapFaceView;
        renderPass.CreateFrameBuffer(attachmentViews, ShadowMapDimensions);

    }
    renderPass.RegisterPipeline<ShadowMappingPipeline>(device, shaderBindingManager);

    return renderPass;
}

auto CreateShadowMappingPass(const nc::graphics::vulkan::Device* device,
                             nc::graphics::vulkan::GpuAllocator* allocator,
                             nc::graphics::vulkan::ShaderBindingManager* shaderBindingManager) -> nc::graphics::vulkan::RenderPass
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

    auto shadowMapExtent = vk::Extent2D{static_cast<uint32_t>(ShadowMapDimensions.x), static_cast<uint32_t>(ShadowMapDimensions.y)};
    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    auto attachments = std::vector<Attachment>{};
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, shadowAttachmentSlots[0].numSamples, shadowAttachmentSlots[0].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled));

    auto sinkViews = std::vector<vk::ImageView>{attachments.at(0).view.get()};
    auto renderPass = RenderPass(vkDevice,
                                 shadowAttachmentSlots,
                                 shadowSubpasses,
                                 std::move(attachments),
                                 AttachmentSize{ShadowMapDimensions, shadowMapExtent},
                                 ClearValueFlags::Depth,
                                 nc::graphics::RenderPassSinkType::UniDirShadowMap,
                                 std::move(sinkViews),
                                 vk::ImageLayout::e
                                 0u);

    const auto attachmentViews = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(0u)};
    renderPass.CreateFrameBuffer(attachmentViews, ShadowMapDimensions);
    renderPass.RegisterPipeline<ShadowMappingPipeline>(device, shaderBindingManager);

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
        auto attachmentViews = std::array<vk::ImageView, 3>
        {
            renderPass.GetAttachmentView(1), // Color Resolve View
            renderPass.GetAttachmentView(0), // Depth View
            swapchainImageView // Swapchain Image at index <swapchainImageIndex>
        };
        renderPass.CreateFrameBuffer(attachmentViews, dimensions);
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
RenderGraph::RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, ShaderResourceBus* shaderResourceBus, Vector2 dimensions)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shaderStorage{shaderStorage},
      m_perFrameRenderGraphs{CreatePerFrameGraphs(m_device, m_swapchain, m_gpuAllocator, dimensions)},
      m_sinkBuffers{{RenderPassSinkType::UniDirShadowMap, shaderResourceBus->CreateRenderPassSinkBuffer(RenderPassSinkType::UniDirShadowMap, 20u, ShaderStage::Fragment, 3u, 2u)}},
      m_sourceCubeMaps{{RenderPassSinkType::OmniDirShadowMap, shaderResourceBus->CreateCubeMapArrayBuffer(10, ShaderStage::Fragment, 0u, 2u, false)}},
      m_dimensions{dimensions},
      m_screenExtent{}
{
}

// Sink outputs from the render graph into shader storage to be consumed by shaders
void RenderGraph::Sink(const RenderPass& renderPass)
{
    OPTICK_CATEGORY("RenderGraph::Sink", Optick::Category::Rendering);
    auto sinkType = renderPass.GetSinkViewsType();
    auto sinkViews = renderPass.GetSinkViews();
    auto sinkLayout = renderPass.GetSinkViewsLayout();

    m_shaderStorage->Sink(sinkViews, sinkType, sinkLayout, m_frameManager->Index());

    if (GetCurrentFrameGraph().isSinkDirty.at(sinkType) && sinkViews.size() > 0)
    {
        m_sinkBuffers.at(sinkType).Update(m_frameManager->Index());
    }
}

void RenderGraph::BuildRenderGraph(const PerFrameRenderStateData& stateData, uint32_t frameIndex)
{
    OPTICK_CATEGORY("RenderGraph::BuildRenderGraph", Optick::Category::Rendering);
    auto& renderGraph = m_perFrameRenderGraphs.at(frameIndex);

    // If lights have been added or removed we need to add or remove shadow passes (one per light)
    if (stateData.omniDirLightsCount != renderGraph.stateData.omniDirLightsCount)
    {
        if (renderGraph.omniDirShadowPasses.size() < stateData.omniDirLightsCount)
        {
            auto shadowCubeMaps = std::vector<nc::asset::CubeMapWithId>{};
            shadowCubeMaps.reserve(renderGraph.omniDirShadowPasses.size());

            while (shadowCubeMaps.size() < stateData.omniDirLightsCount)
            {
                shadowCubeMaps.push_back(nc::asset::CubeMapWithId
                {
                    .cubeMap = nc::asset::CubeMap
                    {
                        .faceSideLength = static_cast<uint32_t>(ShadowMapDimensions.x),
                        .pixelData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
                    },
                    .id = renderGraph.omniDirShadowPasses.size()
                });
            }
            m_sourceCubeMaps.at(RenderPassSinkType::OmniDirShadowMap).Add(shadowCubeMaps, frameIndex);

            renderGraph.omniDirShadowPasses.reserve(stateData.omniDirLightsCount);

            
            std::generate_n(std::back_inserter(renderGraph.omniDirShadowPasses), stateData.omniDirLightsCount - renderGraph.stateData.omniDirLightsCount, [this, frameIndex]()
            {
                return CreateShadowMappingPassOmni(m_device,
                                                   m_gpuAllocator,
                                                   m_shaderBindingManager,
                                                   m_shaderStorage->SourceCubeMapViews(m_sourceCubeMaps.at(RenderPassSinkType::OmniDirShadowMap).Uid(), frameIndex)); // Need cubemap index here too
            });
        }
        else
        {
            while (renderGraph.omniDirShadowPasses.size() > stateData.omniDirLightsCount)
            {
                renderGraph.omniDirShadowPasses.pop_back();
            }
            // m_sourceCubeMaps.at(RenderPassSinkType::OmniDirShadowMap).Remove(, frameIndex); // @todo
        }
    }

    // If lights have been added or removed we need to add or remove shadow passes (one per light)
    if (stateData.uniDirLightsCount != renderGraph.stateData.uniDirLightsCount)
    {
        renderGraph.isSinkDirty.at(RenderPassSinkType::UniDirShadowMap) = true;
        m_sinkBuffers.at(RenderPassSinkType::UniDirShadowMap).Clear();

        if (renderGraph.uniDirShadowPasses.size() < stateData.uniDirLightsCount)
        {
            renderGraph.uniDirShadowPasses.reserve(stateData.uniDirLightsCount);
            std::generate_n(std::back_inserter(renderGraph.uniDirShadowPasses), stateData.uniDirLightsCount - renderGraph.stateData.uniDirLightsCount, [this]()
            {
                return CreateShadowMappingPass(m_device,
                                               m_gpuAllocator,
                                               m_shaderBindingManager);
            });
        }
        else
        {
            while (renderGraph.uniDirShadowPasses.size() > stateData.uniDirLightsCount)
            {
                renderGraph.uniDirShadowPasses.pop_back();
            }
        }
    }

    // All other state data pertains to the lit pass. If any of the comparisons have changed, we'll need to register the appropriate pipelines
    if (stateData != renderGraph.stateData || !renderGraph.isInitialized)
    {
        renderGraph.litPass.UnregisterPipelines();

        #ifdef NC_EDITOR_ENABLED
        if (!renderGraph.isInitialized || stateData.widgetsCount)
            renderGraph.litPass.RegisterPipeline<WireframePipeline>(m_device, m_shaderBindingManager);
        #endif

        if (!renderGraph.isInitialized || stateData.useSkybox)
            renderGraph.litPass.RegisterPipeline<EnvironmentPipeline>(m_device, m_shaderBindingManager);

        if (!renderGraph.isInitialized || stateData.meshRenderersCount)
            renderGraph.litPass.RegisterPipeline<PbrPipeline>(m_device, m_shaderBindingManager);

        if (!renderGraph.isInitialized || stateData.toonRenderersCount)
        {
            renderGraph.litPass.RegisterPipeline<ToonPipeline>(m_device, m_shaderBindingManager);
            renderGraph.litPass.RegisterPipeline<OutlinePipeline>(m_device, m_shaderBindingManager);
        }

        if (!renderGraph.isInitialized || stateData.particlesCount)
            renderGraph.litPass.RegisterPipeline<ParticlePipeline>(m_device, m_shaderBindingManager);

        renderGraph.litPass.RegisterPipeline<UiPipeline>(m_device, m_shaderBindingManager);
    }

    renderGraph.stateData = stateData;
    renderGraph.isInitialized = true;
}

void RenderGraph::Execute(const PerFrameRenderState &frameData, const Vector2& dimensions, const Vector2& screenExtent, uint32_t swapchainImageIndex)
{
    OPTICK_CATEGORY("RenderGraph::Execute", Optick::Category::Rendering);

    auto frame = m_frameManager->CurrentFrameContext();
    const auto cmd = frame->CommandBuffer();
    auto frameIndex = frame->Index();
    auto& renderGraph = GetCurrentFrameGraph();

    SetViewportAndScissorFullWindow(cmd, dimensions);

    auto instanceData = PerFrameInstanceData{};

    for (auto [index, shadowMappingPass] : std::views::enumerate(renderGraph.omniDirShadowPasses))
    {
        instanceData.isOmniDirectional = true;
        instanceData.shadowCasterIndex = static_cast<uint32_t>(index);
        shadowMappingPass.Begin(cmd);
        shadowMappingPass.Execute(cmd, frameData, instanceData, frameIndex);
        shadowMappingPass.End(cmd);
        Sink(shadowMappingPass);
    }
    renderGraph.isSinkDirty.at(RenderPassSinkType::OmniDirShadowMap) = false;

    for (auto [index, shadowMappingPass] : std::views::enumerate(renderGraph.uniDirShadowPasses))
    {
        instanceData.isOmniDirectional = false;
        instanceData.shadowCasterIndex = static_cast<uint32_t>(renderGraph.stateData.omniDirLightsCount + index);
        shadowMappingPass.Begin(cmd);
        shadowMappingPass.Execute(cmd, frameData, instanceData, frameIndex);
        shadowMappingPass.End(cmd);
        Sink(shadowMappingPass);
    }
    renderGraph.isSinkDirty.at(RenderPassSinkType::UniDirShadowMap) = false;

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    renderGraph.litPass.Begin(cmd, swapchainImageIndex);
    renderGraph.litPass.Execute(cmd, frameData, instanceData, frameIndex);
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
        renderGraph.stateData = PerFrameRenderStateData{};
        for (auto& [renderPassSinkType, renderPassSinkBuffer] : m_sinkBuffers)
        {
            renderPassSinkBuffer.Clear();
        }
    }
}

PerFrameRenderGraph::PerFrameRenderGraph(const Device* device,
                                         Swapchain* swapchain,
                                         GpuAllocator* gpuAllocator,
                                         Vector2 dimensions)
    : omniDirShadowPasses{},
      uniDirShadowPasses{},
      litPass{CreateLitPass(device, gpuAllocator, swapchain, dimensions)},
      isSinkDirty{{RenderPassSinkType::UniDirShadowMap, false}},
      isInitialized{false}
{
}
} // namespace nc::graphics::vulkan
