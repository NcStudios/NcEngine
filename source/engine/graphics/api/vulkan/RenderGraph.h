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
class FrameManager;
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
        RenderGraph(FrameManager* frameManager, Registry* registry,const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* descriptorSets, Vector2 dimensions, uint32_t maxLights);

        void RecordDrawCallsOnBuffer(const PerFrameRenderState& frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent);
        void Resize(const Vector2 &dimensions);
        void SinkPostProcessImages();
        auto GetPostProcessImages(PostProcessImageType imageType) -> std::vector<vk::ImageView>;
        auto GetLitPass() const noexcept -> const RenderPass& { return *m_litPass.get(); };
        void CommitResourceLayout();
        void IncrementShadowPassCount();
        void DecrementShadowPassCount();
        void ClearShadowPasses();

    private:
        void SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&);

        FrameManager* m_frameManager;
        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderBindingManager* m_descriptorSets;
        std::vector<std::unique_ptr<RenderPass>> m_shadowMappingPasses;
        Attachment m_dummyShadowMap;
        std::unordered_map<PostProcessImageType, PostProcessViews> m_postProcessImageViews;
        std::unique_ptr<RenderPass> m_litPass;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        uint32_t m_activeShadowMappingPasses;
        uint32_t m_maxLights;
        std::array<bool, MaxFramesInFlight> m_isDescriptorSetLayoutsDirty;
        Connection<const DescriptorSetLayoutsChanged&> m_onDescriptorSetsChanged;
        Connection<PointLight&> m_onCommitPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
};
} // namespace nc
} // namespace graphics
