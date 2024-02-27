#pragma once

#include "core/GpuOptions.h"
#include "ecs/Registry.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/PointLight.h"
#include "renderpasses/RenderPass.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"

#include "utility/Signal.h"

#include <string>

namespace nc
{
class Registry;

namespace graphics
{
class Device;
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderBindingManager;
struct DescriptorSetLayoutsChanged;
class Swapchain;

inline static const std::string LitPassId = "Lit Pass";
inline static const std::string ShadowMappingPassId = "Shadow Mapping Pass";

namespace vulkan
{
    struct PpiaUpdateEventData;
}

struct PostProcessViews
{
    std::array<std::vector<vk::ImageView>, MaxFramesInFlight> perFrameViews;
};

class RenderGraph
{
    public:
        RenderGraph(Registry* registry, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* descriptorSets, Vector2 dimensions, uint32_t maxLights);

        void Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent, uint32_t frameIndex);
        void Resize(const Vector2 &dimensions);

        auto GetPostProcessImages(PostProcessImageType imageType, uint32_t frameIndex) -> std::vector<vk::ImageView>;

        auto GetLitPass() const noexcept -> const RenderPass& { return *m_litPass.get(); };
        void IncrementShadowPassCount();
        void DecrementShadowPassCount();
        void ClearShadowPasses();

    private:
        void SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&) { m_isDescriptorSetLayoutsDirty = true; }
        void MapShaderResources();

        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderBindingManager* m_descriptorSets;
        std::vector<std::unique_ptr<RenderPass>> m_shadowMappingPasses;
        std::unordered_map<PostProcessImageType, PostProcessViews> m_postProcessImageViews;
        std::unique_ptr<RenderPass> m_litPass;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        uint32_t m_activeShadowMappingPasses;
        uint32_t m_maxLights;
        bool m_isDescriptorSetLayoutsDirty;
        Connection<const DescriptorSetLayoutsChanged&> m_onDescriptorSetsChanged;
        Connection<PointLight&> m_onCommitPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
};
} // namespace nc
} // namespace graphics
