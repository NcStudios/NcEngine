#include "RenderGraph.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/ShaderStorage.h"
#include "graphics/api/vulkan/pipelines/EnvironmentPipeline.h"
#include "graphics/api/vulkan/pipelines/OutlinePipeline.h"
#include "graphics/api/vulkan/pipelines/ParticlePipeline.h"
#include "graphics/api/vulkan/pipelines/PbrPipeline.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingTestPipeline.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingUniPipeline.h"
#include "graphics/api/vulkan/pipelines/ToonPipeline.h"
#include "graphics/api/vulkan/pipelines/UiPipeline.h"
#include "graphics/api/vulkan/renderpasses/RenderPassFactories.h"
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
      m_sourceCubeMaps{CreateDummyShadowMaps(1u)},
      m_sourceCubeMapBuffers{{RenderPassSinkType::OmniDirShadowMap, shaderResourceBus->CreateCubeMapArrayBuffer(10, ShaderStage::Fragment, 0u, 2u, false)}},
      m_dimensions{dimensions},
      m_screenExtent{}
{
    m_sourceCubeMapBuffers.at(RenderPassSinkType::OmniDirShadowMap).Add(m_sourceCubeMaps, CubeMapFormat::R32_SFLOAT, CubeMapUsage::ColorAttachment | CubeMapUsage::Sampled, 0u);
    m_sourceCubeMapBuffers.at(RenderPassSinkType::OmniDirShadowMap).Add(m_sourceCubeMaps, CubeMapFormat::R32_SFLOAT, CubeMapUsage::ColorAttachment | CubeMapUsage::Sampled, 1u);
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
            renderGraph.omniDirShadowPasses.reserve(stateData.omniDirLightsCount);
            std::generate_n(std::back_inserter(renderGraph.omniDirShadowPasses), (stateData.omniDirLightsCount - renderGraph.stateData.omniDirLightsCount), [this, frameIndex, cubeMapIndex = 0u]() mutable
            {
                return CreateShadowMappingPassOmni(m_device,
                                                   m_gpuAllocator,
                                                   m_shaderBindingManager,
                                                   m_shaderStorage->SourceCubeMapViews(m_sourceCubeMapBuffers.at(RenderPassSinkType::OmniDirShadowMap).Uid(), frameIndex, cubeMapIndex++)); // Need cubemap index here too
            });
        }
        else
        {
            while (renderGraph.omniDirShadowPasses.size() > stateData.omniDirLightsCount)
            {
                renderGraph.omniDirShadowPasses.pop_back();
            }
            // m_sourceCubeMapBuffers.at(RenderPassSinkType::OmniDirShadowMap).Remove(, frameIndex); // @todo
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

        // if (!renderGraph.isInitialized || stateData.toonRenderersCount || stateData.meshRenderersCount)
        // {
        //     renderGraph.litPass.RegisterPipeline<ShadowMappingTestPipeline>(m_device, m_shaderBindingManager);
        // }

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

    SetViewportAndScissorFullWindow(cmd, nc::Vector2(1024, 1024));

    auto instanceData = PerFrameInstanceData{};

    for (auto& shadowMappingPass : renderGraph.omniDirShadowPasses)
    {
        for (auto shadowCubeFaceIndex : std::views::iota(0u, 6u))
        {
            instanceData.isOmniDirectional = true;
            instanceData.shadowCasterIndex = static_cast<uint32_t>(shadowCubeFaceIndex);
            shadowMappingPass.Begin(cmd, static_cast<uint32_t>(shadowCubeFaceIndex));
            shadowMappingPass.Execute(cmd, frameData, instanceData, frameIndex);
            shadowMappingPass.End(cmd);
        }
    }

    // for (auto [index, shadowMappingPass] : std::views::enumerate(renderGraph.uniDirShadowPasses))
    // {
    //     instanceData.isOmniDirectional = false;
    //     instanceData.shadowCasterIndex = static_cast<uint32_t>(renderGraph.stateData.omniDirLightsCount + index);
    //     shadowMappingPass.Begin(cmd);
    //     shadowMappingPass.Execute(cmd, frameData, instanceData, frameIndex);
    //     shadowMappingPass.End(cmd);
    //     Sink(shadowMappingPass);
    // }
    // renderGraph.isSinkDirty.at(RenderPassSinkType::UniDirShadowMap) = false;

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
