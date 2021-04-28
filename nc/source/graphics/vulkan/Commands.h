#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Base; class Swapchain;

    class Commands
    {
        public:
            Commands(Base* base, const vulkan::Swapchain& swapchain);
            ~Commands();

            std::vector<vk::CommandBuffer>* GetCommandBuffers();
            void FreeCommandBuffers();
            void SubmitRenderCommand(uint32_t imageIndex);
            static void SubmitCopyCommandImmediate(const vulkan::Base& base, const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, const vk::DeviceSize size);

        private:
            // External members
            Base* m_base;
            const Swapchain& m_swapchain;
            const std::vector<vk::Semaphore>& m_renderReadySemaphores;
            const std::vector<vk::Semaphore>& m_presentReadySemaphores;
            const std::vector<vk::Fence>& m_framesInFlightFences;
            const std::vector<vk::Fence>& m_imagesInFlightFences;

            // Internal members
            std::vector<vk::CommandBuffer> m_commandBuffers;
    };
}