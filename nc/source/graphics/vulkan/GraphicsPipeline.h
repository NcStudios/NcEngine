#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Device; class RenderPass;

    class GraphicsPipeline
    {
        public:
            GraphicsPipeline(const vulkan::Device* device, const vulkan::RenderPass* renderPass); // &todo: Change from passing in a single render pass

        private:
            vk::UniquePipelineLayout m_pipelineLayout;
            vk::UniquePipeline m_pipeline;
            vk::ShaderModule CreateShaderModule(const std::vector<char>& code, const vulkan::Device* device);
    };
}