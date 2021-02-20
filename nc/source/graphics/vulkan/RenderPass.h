#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Device;

    class RenderPass
    {
        public:
            RenderPass(const vulkan::Device& device);
            const vk::RenderPass& GetRenderPass() const noexcept;
        private:
            vk::UniqueRenderPass m_renderPass;
    };
}