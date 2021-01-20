#pragma once

#include "vulkan\Instance.h"

namespace nc::graphics
{
    class Graphics2
    {
        public:
            Graphics2();

        private:
            vk::UniqueInstance m_instance;
    };
}