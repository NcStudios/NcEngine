#pragma once

#include "core/GpuOptions.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/PointLight.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "graphics/SpotLight.h"

#include "utility/Signal.h"

#include <string>

namespace nc
{
class Registry;

namespace graphics
{
struct PpiaUpdateEventData;

namespace vulkan
{
class Device;
class FrameManager;
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderBindingManager;
struct ShaderStorage;
class Swapchain;

struct PerFrameRenderGraph
{
    PerFrameRenderGraph(const Device* device,
                        Swapchain* swapchain,
                        GpuAllocator* gpuAllocator,
                        Vector2 dimensions,
                        uint32_t index);

    std::vector<RenderPass> shadowPasses; // One per light
    RenderPass litPass;
    PerFrameRenderStateData stateData;
};

class RenderGraph
{
    public:
        RenderGraph(FrameManager* frameManager, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, ShaderStorage* shaderStorage, Vector2 dimensions, uint32_t maxLights);

        void RecordDrawCallsOnBuffer(const PerFrameRenderState& frameData, const Vector2& dimensions, const Vector2& screenExtent);
        void Resize(const Vector2 &dimensions);
        void SinkPostProcessImages();
        auto GetLitPass() const noexcept -> const RenderPass& { return m_perFrameRenderGraphs.at(0).litPass; };
        void BuildRenderGraph(PerFrameRenderStateData stateData, uint32_t frameIndex);

    private:

        // External dependencies
        FrameManager* m_frameManager;
        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderBindingManager* m_shaderBindingManager;
        ShaderStorage* m_shaderStorage;

        Attachment m_dummyShadowMap;
        std::array<PerFrameRenderGraph, MaxFramesInFlight> m_perFrameRenderGraphs;

        // Screen size
        Vector2 m_dimensions;
        Vector2 m_screenExtent;

        // State tracking
        uint32_t m_maxLights;
};
} // namespace nc
} // namespace graphics
} // namespace vulkan
