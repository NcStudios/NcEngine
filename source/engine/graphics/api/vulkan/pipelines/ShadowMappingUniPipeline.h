#pragma once

#include "IPipeline.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingUniPipeline : public IPipeline
    {
        public:
            ShadowMappingUniPipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
            ~ShadowMappingUniPipeline() noexcept;

            void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData& instanceData) override;

        private:
            ShaderBindingManager* m_shaderBindingManager;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan
