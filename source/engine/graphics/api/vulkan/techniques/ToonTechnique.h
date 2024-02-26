#pragma once

#include "ITechnique.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Device;
class ShaderBindingManager;

class ToonTechnique : public ITechnique
{
public:
    ToonTechnique(const Device& device, ShaderBindingManager* descriptorSets, vk::RenderPass* renderPass);
    ~ToonTechnique() noexcept;

    bool CanBind(const PerFrameRenderState& frameData) override;
    void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;

    bool CanRecord(const PerFrameRenderState& frameData) override;
    void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

    void Clear() noexcept;

private:
    ShaderBindingManager* m_descriptorSets;
    vk::UniquePipeline m_pipeline;
    vk::UniquePipelineLayout m_pipelineLayout;
};
}