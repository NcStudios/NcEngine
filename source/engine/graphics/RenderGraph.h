#pragma once

#include "graphics/GpuAssetsStorage.h"
#include "graphics/renderpasses/RenderPass.h"

#include <string>

namespace nc::graphics
{
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderDescriptorSets;
class Swapchain;

inline static const std::string LitPassId = "Lit Pass";

class RenderGraph
{
    public:
        RenderGraph(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions);

        void Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, const MeshStorage &meshStorage, uint32_t frameBufferIndex, Vector2 dimensions);
        void Resize(const Vector2 &dimensions);

        auto GetRenderPass(const std::string& uid) -> const RenderPass&;
        void AddRenderPass(RenderPass renderPass);
        void RemoveRenderPass(const std::string& uid);

    private:
        vk::Device m_device;
        Swapchain* m_swapchain;
        GpuOptions* m_gpuOptions;
        GpuAllocator* m_gpuAllocator;
        ShaderDescriptorSets* m_descriptorSets;
        std::vector<RenderPass> m_renderPasses;
        Vector2 m_dimensions;
};
} // namespace nc