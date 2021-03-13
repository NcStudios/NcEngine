#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class RenderPass; class FrameBuffers; class GraphicsPipeline; class VertexBuffer; class IndexBuffer;

    class Commands
    {
        public:
            Commands(const vulkan::Base& base, 
                     const std::vector<vk::Semaphore>& renderReadySemaphores, 
                     const std::vector<vk::Semaphore>& presentReadySemaphores, 
                     const std::vector<vk::Fence>& framesInFlightFences, 
                     const std::vector<vk::Fence>& imagesInFlightFences);

            void RecordRenderCommand(const vulkan::Base& base, const vulkan::RenderPass& renderPass, const FrameBuffers& frameBuffers, const vulkan::GraphicsPipeline& pipeline, const vulkan::VertexBuffer& vertexBuffer, const vulkan::IndexBuffer& indexBuffer);
            void SubmitRenderCommand(const vulkan::Base& base, uint32_t imageIndex);

            void SubmitCopyCommandImmediate(const vulkan::Base& base, const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);

        private:
            std::vector<vk::CommandBuffer> m_commandBuffers;
            const std::vector<vk::Semaphore>& m_renderReadySemaphores;
            const std::vector<vk::Semaphore>& m_presentReadySemaphores;
            const std::vector<vk::Fence>& m_framesInFlightFences;
            const std::vector<vk::Fence>& m_imagesInFlightFences;
    };
}