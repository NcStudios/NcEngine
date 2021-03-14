#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Base; class Swapchain;

    class GraphicsPipeline
    {
        public:
            GraphicsPipeline(const vulkan::Base& base, const vulkan::Swapchain& swapchain);
            const vk::Pipeline& GetPipeline() const;

        private:
            vk::UniquePipelineLayout m_pipelineLayout;
            vk::UniquePipeline m_pipeline;
            vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, const vulkan::Base& base);
    };
}