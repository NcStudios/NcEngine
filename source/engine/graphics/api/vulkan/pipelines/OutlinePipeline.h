#pragma once

#include "IPipeline.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
class Device;
class ShaderBindingManager;

class OutlinePipeline : public IPipeline
{
public:
    OutlinePipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
    ~OutlinePipeline() noexcept;

    void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
    void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;
    void Clear() noexcept;

private:
    ShaderBindingManager* m_shaderBindingManager;
    vk::UniquePipeline m_pipeline;
    vk::UniquePipelineLayout m_pipelineLayout;
};
} // namespace nc::graphics::vulkan
