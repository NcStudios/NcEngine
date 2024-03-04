#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ITechnique.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Device;
class ShaderDescriptorSets;

struct WireframeVertexPushConstants
{
    DirectX::XMMATRIX model;
    DirectX::XMMATRIX viewProjection;
};

struct WireframeFragmentPushConstants
{
    alignas(16) Vector4 color;
};

class WireframeTechnique : public ITechnique
{
    public:
        WireframeTechnique(const Device& device, ShaderDescriptorSets*, vk::RenderPass* renderPass);
        ~WireframeTechnique() noexcept;

        bool CanBind(const PerFrameRenderState& frameData) override;
        void Bind(vk::CommandBuffer* cmd) override;

        bool CanRecord(const PerFrameRenderState& frameData) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

    private:
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
};
} // namespace nc::graphics
#endif
