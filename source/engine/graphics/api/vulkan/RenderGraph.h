#pragma once

#include "GpuAssetsStorage.h"
#include "core/GpuOptions.h"
#include "renderpasses/RenderPass.h"

#include <string>

namespace nc::graphics
{
class Device;
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderDescriptorSets;
class Swapchain;

inline static const std::string LitPassId = "Lit Pass";

class RenderGraph
{
    public:
        RenderGraph(const Device& device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions, Vector2 screenExtent);

        void Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, const MeshStorage &meshStorage, uint32_t frameBufferIndex, Vector2 dimensions, Vector2 screenExtent);
        void Resize(const Device& device, const Vector2 &dimensions);

        auto GetRenderPass(const std::string& uid) -> const RenderPass&;
        void AddRenderPass(RenderPass renderPass);
        void RemoveRenderPass(const std::string& uid);

    private:
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderDescriptorSets* m_descriptorSets;
        std::vector<RenderPass> m_renderPasses;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
};
} // namespace nc