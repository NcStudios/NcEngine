#pragma once

#include "core/GpuOptions.h"
#include "renderpasses/RenderPass.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"

#include "utility/Signal.h"

#include <string>

namespace nc::graphics
{
class Device;
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderDescriptorSets;
struct DescriptorSetLayoutsChanged;
class Swapchain;

inline static const std::string LitPassId = "Lit Pass";
inline static const std::string ShadowMappingPassId = "Shadow Mapping Pass";

namespace vulkan
{
    struct PpiaUpdateEventData;
}

class RenderGraph
{
    public:
        RenderGraph(const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions, uint32_t maxLights);

        void Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent, uint32_t frameIndex);
        void Resize(const Vector2 &dimensions);

        auto GetPostProcessImages(PostProcessImageType imageType) -> std::vector<vk::ImageView>;

        auto GetShadowPasses() const noexcept -> const std::vector<RenderPass>& { return m_shadowMappingPasses; };
        auto GetLitPass() const noexcept -> const RenderPass& { return m_litPass; };
        void IncrementShadowPassCount();
        void DecrementShadowPassCount();
        void ClearShadowPasses();

    private:
        void SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&) { m_isDescriptorSetLayoutsDirty = true; }
        void MapShaderResources();

        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderDescriptorSets* m_descriptorSets;
        std::vector<RenderPass> m_shadowMappingPasses;
        std::unordered_map<PostProcessImageType, std::vector<vk::ImageView>> m_postProcessImageViews;
        RenderPass m_litPass;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        uint32_t m_activeShadowMappingPasses;
        uint32_t m_maxLights;
        bool m_isDescriptorSetLayoutsDirty;
        Connection<const DescriptorSetLayoutsChanged&> m_onDescriptorSetsChanged;
};
} // namespace nc::graphics
