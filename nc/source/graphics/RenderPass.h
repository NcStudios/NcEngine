#pragma once

#include "Graphics.h"
#include "Attachment.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>

namespace nc::graphics
{
    class RenderPass
    {
        public:
            RenderPass(Graphics* graphics, 
                       const std::vector<vk::SubpassDependency>& subpassDependencies,
                       const std::vector<Attachment>& attachments);

            void Begin(vk::CommandBuffer* cmd);
            void End(vk::CommandBuffer* cmd);
            
        private:
            vk::UniqueRenderPass m_renderPass;
            vk::UniqueFramebuffer m_frameBuffer;
    };
}