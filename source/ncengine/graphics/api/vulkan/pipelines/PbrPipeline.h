#pragma once

#include "IPipeline.h"
#include "graphics/api/vulkan/NcVulkan.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    class PbrPipeline : public IPipeline
    {
    public:
        PbrPipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass);
        ~PbrPipeline() noexcept;

        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;
        void Clear() noexcept;

    private:
        ShaderBindingManager* m_shaderBindingManager;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan
