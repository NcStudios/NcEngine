#pragma once

#include "ITechnique.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class ShaderBindingManager;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingTechnique : public ITechnique
    {
        public:
            ShadowMappingTechnique(vk::Device device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass, uint32_t shadowCasterIndex, bool isOmniDirectional);
            ~ShadowMappingTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
            
            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            ShaderBindingManager* m_shaderBindingManager;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
            uint32_t m_shadowCasterIndex;
            bool m_enabled;
            bool m_isOmniDirectional;
    };
} // namespace nc::graphics::vulkan
