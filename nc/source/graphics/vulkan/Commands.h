#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Device; class RenderPass; class FrameBuffers; class GraphicsPipeline;

    class Commands
    {
        public:
            Commands(const vulkan::Device& device, 
                     const std::vector<vk::Semaphore>& renderReadySemaphores, 
                     const std::vector<vk::Semaphore>& presentReadySemaphores, 
                     const std::vector<vk::Fence>& framesInFlightFences, 
                     const std::vector<vk::Fence>& imagesInFlightFences);

            void RecordRenderPass(const vulkan::Device& device, const vulkan::RenderPass& renderPass, const FrameBuffers& frameBuffers, const vulkan::GraphicsPipeline& pipeline);
            void SubmitCommandBuffer(const vulkan::Device& device, uint32_t imageIndex);

        private:
            std::vector<vk::CommandBuffer> m_commandBuffers;
            const std::vector<vk::Semaphore>& m_renderReadySemaphores;
            const std::vector<vk::Semaphore>& m_presentReadySemaphores;
            const std::vector<vk::Fence>& m_framesInFlightFences;
            const std::vector<vk::Fence>& m_imagesInFlightFences;
    };
}