#pragma once

#include "ECS.h"
#include "ITechnique.h"

#include "directx/math/DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics; class Commands; class Base; class Swapchain;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingTechnique : public ITechnique
    {
        public:
            ShadowMappingTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~ShadowMappingTechnique();

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;
            
            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
            bool m_enabled;
    };
}