#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Device; class RenderPass;

    class FrameBuffers
    {
        public:
            FrameBuffers(const Device* device, const RenderPass* renderPass);
        private:
            std::vector<vk::UniqueFramebuffer> m_framebuffers;
    };
}