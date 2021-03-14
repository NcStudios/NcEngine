#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class FrameBuffers; class GraphicsPipeline; class VertexBuffer; class IndexBuffer; class Swapchain;

    class Commands
    {
        public:
            Commands(const vulkan::Base& base, const vulkan::Swapchain& swapchain);

            void RecordRenderCommand(const vulkan::GraphicsPipeline& pipeline, const vulkan::Swapchain& swapchain, const vulkan::VertexBuffer& vertexBuffer, const vulkan::IndexBuffer& indexBuffer);
            void SubmitRenderCommand(uint32_t imageIndex);
            void SubmitCopyCommandImmediate(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);

        private:
            // External members
            const Base& m_base;
            const Swapchain& m_swapchain;
            const std::vector<vk::Semaphore>& m_renderReadySemaphores;
            const std::vector<vk::Semaphore>& m_presentReadySemaphores;
            const std::vector<vk::Fence>& m_framesInFlightFences;
            const std::vector<vk::Fence>& m_imagesInFlightFences;

            // Internal members
            std::vector<vk::CommandBuffer> m_commandBuffers;
    };
}