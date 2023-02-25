#include "Commands.h"
#include "Core.h"
#include "PerFrameGpuContext.h"
#include "Swapchain.h"

namespace nc::graphics
{
    Commands::Commands(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, Swapchain* swapchain)
        : m_device{logicalDevice},
          m_physicalDevice{physicalDevice},
          m_surface{surface},
          m_swapchain{swapchain},
          m_graphicsQueue{},
          m_presentQueue{}
    {
        auto queueFamilies = QueueFamilyIndices(m_physicalDevice, m_surface);
        m_graphicsQueue = m_device.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::GraphicsFamily), 0);
        m_presentQueue = m_device.getQueue(queueFamilies.GetQueueFamilyIndex(QueueFamilyType::PresentFamily), 0);
    }

    void Commands::ExecuteCommand(std::function<void(vk::CommandBuffer cmd)>&& function)
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        auto queueFamilies = nc::graphics::QueueFamilyIndices(m_physicalDevice, m_surface);
        poolInfo.setQueueFamilyIndex(queueFamilies.GetQueueFamilyIndex(nc::graphics::QueueFamilyType::GraphicsFamily));
        auto commandPool = m_device.createCommandPoolUnique(poolInfo);

        auto allocInfo = vk::CommandBufferAllocateInfo{};
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(commandPool.get());
        allocInfo.setCommandBufferCount(1);

        auto tempCommandBuffers = m_device.allocateCommandBuffers(allocInfo);
        auto tempCommandBuffer = tempCommandBuffers[0];

        auto beginInfo = vk::CommandBufferBeginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        tempCommandBuffer.begin(beginInfo);
        {
            function(tempCommandBuffer);
        }
        tempCommandBuffer.end();

        auto submitInfo = vk::SubmitInfo{};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&tempCommandBuffer);
        m_graphicsQueue.submit(submitInfo, nullptr);
        m_graphicsQueue.waitIdle();
        m_device.freeCommandBuffers(commandPool.get(), tempCommandBuffer);
    }

    const vk::Queue& Commands::GetCommandQueue(QueueFamilyType type) const noexcept
    {
        return type == QueueFamilyType::GraphicsFamily ? m_graphicsQueue : m_presentQueue;
    }
}  // namespace nc::graphics