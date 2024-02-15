#pragma once

#include "ITechnique.h"
#include "ecs/Component.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

#include <vector>
#include <span>

namespace nc::graphics
{
class Device;
class ShaderDescriptorSets;

class EnvironmentTechnique : public ITechnique
{
    public:
        EnvironmentTechnique(const Device& device, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass);
        ~EnvironmentTechnique() noexcept;

        bool CanBind(const PerFrameRenderState& frameData) override;
        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;

        bool CanRecord(const PerFrameRenderState& frameData) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;
        
        void Clear() noexcept;

    private:
        ShaderDescriptorSets* m_descriptorSets;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
};
} // namespace nc::graphics
