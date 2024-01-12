#pragma once

#include "ITechnique.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class ShaderDescriptorSets;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingTechnique : public ITechnique
    {
        public:
            ShadowMappingTechnique(vk::Device device, ShaderDescriptorSets* descriptorSets, vk::RenderPass renderPass, uint32_t shadowCasterIndex);
            ~ShadowMappingTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;
            
            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;
            bool IsShadowMapTechnique() override {return true;} //@todo: total HACK.

        private:
            ShaderDescriptorSets* m_descriptorSets;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
            bool m_enabled;
            uint32_t m_shadowCasterIndex;
    };
}