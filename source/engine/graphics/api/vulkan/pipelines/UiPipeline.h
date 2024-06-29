#pragma once

#include "IPipeline.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    class UiPipeline : public IPipeline
    {
        public:
            UiPipeline(const Device& device, ShaderBindingManager*, vk::RenderPass renderPass);
            ~UiPipeline() noexcept;

            void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&) override;

        private:
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan
