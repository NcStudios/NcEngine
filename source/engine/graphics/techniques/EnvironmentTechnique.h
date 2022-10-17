#pragma once

#include "DirectXMath.h"
#include "ecs/Component.h"
#include "graphics/resources/ShaderDescriptorSets.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>

namespace nc::graphics
{
    class Graphics; class GpuOptions; class Swapchain;

    class EnvironmentTechnique : public ITechnique
    {
        public:
            EnvironmentTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~EnvironmentTechnique() noexcept;
            
            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;
            
            void Clear() noexcept;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            GpuOptions* m_gpuOptions;
            ShaderDescriptorSets* m_descriptorSets;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}