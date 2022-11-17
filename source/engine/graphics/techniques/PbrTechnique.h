#pragma once

#include "graphics/resources/ShaderDescriptorSets.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class GpuOptions; class ShaderDescriptorSets;

    class PbrTechnique : public ITechnique
    {
    public:
        PbrTechnique(vk::Device device, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass);
        ~PbrTechnique() noexcept;

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