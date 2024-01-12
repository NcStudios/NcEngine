#pragma once

#include "ITechnique.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Device;
class ShaderDescriptorSets;

class ToonTechnique : public ITechnique
{
public:
    ToonTechnique(const Device& device, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass);
    ~ToonTechnique() noexcept;

    bool CanBind(const PerFrameRenderState& frameData) override;
    void Bind(vk::CommandBuffer* cmd) override;

    bool CanRecord(const PerFrameRenderState& frameData) override;
    void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

    void Clear() noexcept;

    bool IsShadowMapTechnique() override {return false;} //@todo: total HACK.

private:
    ShaderDescriptorSets* m_descriptorSets;
    vk::UniquePipeline m_pipeline;
    vk::UniquePipelineLayout m_pipelineLayout;
};
}