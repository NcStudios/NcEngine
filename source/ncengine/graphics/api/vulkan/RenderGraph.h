#pragma once

#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/RenderPassSinkBufferHandle.h"

namespace nc::graphics
{
struct RpsUpdateEventData;
struct ShaderResourceBus;

namespace vulkan
{
class Device;
class GpuAllocator;
class PerFrameGpuContext;
class ShaderBindingManager;
struct ShaderStorage;
class Swapchain;

struct PerFrameRenderGraph
{
    PerFrameRenderGraph(const Device* device,
                        Swapchain* swapchain,
                        GpuAllocator* gpuAllocator,
                        Vector2 dimensions);

    std::vector<RenderPass> shadowPasses; // One per light
    RenderPass litPass;
    PerFrameRenderStateData stateData;
    std::unordered_map<RenderPassSinkType, bool> isSinkDirty;
    bool isInitialized;
};

class RenderGraph
{
    public:
        RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, ShaderResourceBus* shaderResourceBus, Vector2 dimensions);

        void Execute(const PerFrameRenderState& frameData, const Vector2& dimensions, const Vector2& screenExtent, uint32_t swapchainImageIndex);
        void Resize(const Vector2 &dimensions);
        void Clear();
        void Sink(const RenderPass& renderPass);
        auto GetLitPass() const noexcept -> const RenderPass& { return m_perFrameRenderGraphs.at(0).litPass; };
        void BuildRenderGraph(const PerFrameRenderStateData& stateData, uint32_t frameIndex);

    private:
        auto GetCurrentFrameGraph() -> PerFrameRenderGraph& { return m_perFrameRenderGraphs.at(m_frameManager->CurrentFrameContext()->Index()); }

        // External dependencies
        FrameManager* m_frameManager;
        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderBindingManager* m_shaderBindingManager;
        ShaderStorage* m_shaderStorage;

        // Internal data
        std::array<PerFrameRenderGraph, MaxFramesInFlight> m_perFrameRenderGraphs;
        std::unordered_map<RenderPassSinkType, RenderPassSinkBufferHandle> m_sinkBuffers;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        uint32_t m_maxLights;
};
} // namespace nc::graphics
} // namespace vulkan
