#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ITechnique.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

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

class WireframeTechnique : public ITechnique
{
    public:
        WireframeTechnique(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
        ~WireframeTechnique() noexcept;

        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;

    private:
        ShaderBindingManager* m_shaderBindingManager;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
};
} // namespace nc::graphics::vulkan
#endif
