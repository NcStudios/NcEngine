#ifdef NC_EDITOR_ENABLED
#pragma once

#include "IPipeline.h"
#include "graphics/api/vulkan/NcVulkan.h"

#include "DirectXMath.h"

namespace nc::graphics::vulkan
{
class Device;
class ShaderBindingManager;

struct WireframeVertexPushConstants
{
    DirectX::XMMATRIX model;
};

struct WireframeFragmentPushConstants
{
    alignas(16) Vector4 color;
};

class WireframePipeline : public IPipeline
{
    public:
        WireframePipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
        ~WireframePipeline() noexcept;

        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;

    private:
        ShaderBindingManager* m_shaderBindingManager;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
};
} // namespace nc::graphics::vulkan
#endif
