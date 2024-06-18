#pragma once

#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"

namespace nc::graphics
{
struct PpiaUpdateEventData;

namespace vulkan
{
class Device;
class GpuAllocator;
class PerFrameGpuContext;
class ShaderBindingManager;
class ShaderResourceBus;
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
    std::unordered_map<PostProcessImageType, bool> updateRenderTargets;
    bool isInitialized;
};

class RenderGraph
{
    public:
        RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, ShaderResourceBus& shaderResourceBus, Vector2 dimensions);

        void RecordDrawCallsOnBuffer(const PerFrameRenderState& frameData, const Vector2& dimensions, const Vector2& screenExtent, uint32_t swapchainImageIndex);
        void Resize(const Vector2 &dimensions);
        void Clear();
        void SinkRenderTargets(const RenderPass& renderPass);
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
        Attachment m_dummyShadowMap;
        std::array<PerFrameRenderGraph, MaxFramesInFlight> m_perFrameRenderGraphs;
        std::unordered_map<PostProcessImageType, PPImageArrayBufferHandle> m_renderTargetsBuffers;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        uint32_t m_maxLights;
};
} // namespace nc::graphics
} // namespace vulkan
