#pragma once

#include "TechniqueType.h"

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class Swapchain; class FrameManager; class Commands;

    class TechniqueBase
    {
        public:
            TechniqueBase(TechniqueType techniqueType, vulkan::FrameManager* frameManager);
            vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, const vulkan::Base& base);
            static std::vector<uint32_t> ReadShader(const std::string& filename);
            TechniqueType GetType() const noexcept;

        protected:
            ~TechniqueBase();

            // External members
            const Base& m_base;
            const Swapchain& m_swapchain;
            Commands* m_commands;
            FrameManager* m_frameManager;

            // Internal members
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
            std::vector<vk::RenderPass> m_renderPasses;
            vk::DescriptorSetLayout m_descriptorSetLayout;
            TechniqueType m_type;
    };
}