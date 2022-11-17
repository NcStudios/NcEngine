#pragma once

#include "DirectXMath.h"
#include "ecs/Component.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>

namespace nc::graphics
{
    class GpuOptions; class ShaderDescriptorSets;

    class EnvironmentTechnique : public ITechnique
    {
        public:
            EnvironmentTechnique(vk::Device device, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass);
            ~EnvironmentTechnique() noexcept;
            
            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;
            
            void Clear() noexcept;

        private:
            ShaderDescriptorSets* m_descriptorSets;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}