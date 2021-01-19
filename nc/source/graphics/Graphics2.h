#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics
{
    class Graphics2
    {
        public:
            Graphics2();

        private:
            void InitializeVulkan();
            vk::UniqueInstance m_vulkan;
    };
}