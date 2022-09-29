#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include "vk/QueueFamily.h"
#include <vector>

namespace nc::graphics
{
struct Core;
class PerFrameGpuContext;
class Swapchain;
struct IndexBuffer;
struct VertexBuffer;

    class Commands
    {
        public:
            Commands(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, Swapchain* swapchain);

            void SubmitQueue(PerFrameGpuContext* currentFrame);
            void ExecuteCommand(std::function<void(vk::CommandBuffer cmd)>&& function);
            const vk::Queue& GetCommandQueue(QueueFamilyType type) const noexcept;

        private:
            // External members
            vk::Device m_device;
            vk::PhysicalDevice m_physicalDevice;
            vk::SurfaceKHR m_surface;
            Swapchain* m_swapchain;

            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;
    };
} // namespace nc::graphics