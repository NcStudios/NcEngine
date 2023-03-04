#pragma once

#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class GpuOptions; class ShaderDescriptorSets;

    class UiTechnique : public ITechnique
    {
        public:
            UiTechnique(vk::Device device, GpuOptions* gpuOptions, ShaderDescriptorSets*, vk::RenderPass* renderPass);
            ~UiTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}