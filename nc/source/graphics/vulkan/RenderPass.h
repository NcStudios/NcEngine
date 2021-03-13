#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Base;

    class RenderPass
    {
        public:
            RenderPass(const vulkan::Base& base);
            const vk::RenderPass& GetRenderPass() const noexcept;
        private:
            vk::UniqueRenderPass m_renderPass;
    };
}