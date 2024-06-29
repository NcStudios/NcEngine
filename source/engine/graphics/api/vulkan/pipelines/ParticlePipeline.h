#pragma once

#include "IPipeline.h"
#include "ecs/Component.h"
#include "particle/EmitterState.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    struct ParticlePushConstants
    {
        DirectX::XMMATRIX viewProjection;
    };

    class ParticlePipeline : public IPipeline
    {
    public:
        ParticlePipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
        ~ParticlePipeline() noexcept;

        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;
        void Clear() noexcept;

    private:
        ShaderBindingManager* m_shaderBindingManager;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan