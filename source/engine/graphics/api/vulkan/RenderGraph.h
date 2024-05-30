#pragma once

#include "core/GpuOptions.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "graphics/GraphicsConstants.h"
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
struct DescriptorSetLayoutsChanged;
class Device;
class FrameManager;
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderBindingManager;
class Swapchain;

inline static const std::string LitPassId = "Lit Pass";
inline static const std::string ShadowMappingPassId = "Shadow Mapping Pass";

struct PostProcessViews
{
    std::array<std::vector<vk::ImageView>, MaxFramesInFlight> perFrameViews;
};

class RenderGraph
{
    public:
        RenderGraph(FrameManager* frameManager, Registry* registry,const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, Vector2 dimensions, uint32_t maxLights);

        void RecordDrawCallsOnBuffer(const PerFrameRenderState& frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent);
        void Resize(const Vector2 &dimensions);
        void SinkPostProcessImages();
        auto GetPostProcessImages(PostProcessImageType imageType) -> const std::vector<vk::ImageView>&;
        auto GetLitPass() const noexcept -> const RenderPass& { return m_litPass.at(0u); };
        void CommitResourceLayout();
        void IncrementShadowPassCount(bool isOmniDirectional);
        void DecrementShadowPassCount(bool isOmniDirectional);
        void ClearShadowPasses() noexcept;

    private:
        void SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&);

        // External dependencies
        FrameManager* m_frameManager;
        const Device* m_device;
        Swapchain* m_swapchain;
        GpuAllocator* m_gpuAllocator;
        ShaderBindingManager* m_shaderBindingManager;

        // Render Passes
        std::array<std::vector<RenderPass>, MaxFramesInFlight> m_shadowMappingPasses;
        std::array<RenderPass, MaxFramesInFlight> m_litPass;

        // Post process images
        std::unordered_map<PostProcessImageType, PostProcessViews> m_postProcessImageViews;
        Attachment m_dummyShadowMap;

        // Signal connections
        Connection<const DescriptorSetLayoutsChanged&> m_onDescriptorSetsChanged;
        Connection<PointLight&> m_onCommitOmniLight;
        Connection<Entity> m_onRemoveOmniLight;
        Connection<SpotLight&> m_onCommitUniLight;
        Connection<Entity> m_onRemoveUniLight;

        // Screen size
        Vector2 m_dimensions;
        Vector2 m_screenExtent;

        // State tracking
        uint32_t m_omniDirLightCount;
        uint32_t m_uniDirLightCount;
        uint32_t m_maxLights;
        std::array<bool, MaxFramesInFlight> m_isDescriptorSetLayoutsDirty;
};
} // namespace nc
} // namespace graphics
} // namespace vulkan
