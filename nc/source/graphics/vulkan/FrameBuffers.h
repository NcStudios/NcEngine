#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Device; class RenderPass;

    class FrameBuffers
    {
        public:
            FrameBuffers(const Device* device, const RenderPass* renderPass);
            const vk::Framebuffer* GetFrameBuffer(uint32_t index) const;

        private:
            std::vector<vk::UniqueFramebuffer> m_framebuffers;
    };
}