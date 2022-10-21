#pragma once

#include "graphics/resources/ShaderDescriptorSets.h"
#include "ITechnique.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingTechnique : public ITechnique
    {
        public:
            ShadowMappingTechnique(vk::Device device, Graphics* graphics, vk::RenderPass* renderPass);
            ~ShadowMappingTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;
            
            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            ShaderDescriptorSets* m_descriptorSets;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
            bool m_enabled;
    };
}