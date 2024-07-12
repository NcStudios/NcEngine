#pragma once

#include "IPipeline.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    struct ShadowMappingOmniPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
        uint32_t lightIndex;
    };

    class ShadowMappingOmniPipeline : public IPipeline
    {
        public:
            ShadowMappingOmniPipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
            ~ShadowMappingOmniPipeline() noexcept;

            void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData& instanceData) override;

        private:
            ShaderBindingManager* m_shaderBindingManager;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan
