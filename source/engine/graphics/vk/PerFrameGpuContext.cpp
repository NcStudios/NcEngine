#include "PerFrameGpuContext.h"
#include "QueueFamily.h"
#include "utility/NcError.h"

namespace
{
vk::UniqueCommandPool CreateCommandPool(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::SurfaceKHR surface)
{
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    auto queueFamilies = nc::graphics::QueueFamilyIndices(physicalDevice, surface);
    poolInfo.setQueueFamilyIndex(queueFamilies.GetQueueFamilyIndex(nc::graphics::QueueFamilyType::GraphicsFamily));
    return logicalDevice.createCommandPoolUnique(poolInfo);
}

vk::UniqueCommandBuffer CreateCommandBuffer(vk::Device device, vk::CommandPool commandPool)
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(commandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandBufferCount(1);
    return std::move(device.allocateCommandBuffersUnique(allocInfo).front());
}
}

namespace nc::graphics
{
PerFrameGpuContext::PerFrameGpuContext(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
    : m_device{logicalDevice},
      m_imageAvailableSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_renderFinishedSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_inFlightFence{m_device.createFenceUnique(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled})},
      m_commandPool{CreateCommandPool(physicalDevice, m_device, surface)},
      m_commandBuffer{CreateCommandBuffer(m_device, m_commandPool.get())}
{
}

void PerFrameGpuContext::Wait()
{
    if (m_device.waitForFences(m_inFlightFence.get(), true, UINT64_MAX) != vk::Result::eSuccess)
    {
        throw NcError("Could not wait for fences to complete.");
    }
}

void PerFrameGpuContext::Reset() noexcept
{
    m_device.resetFences(m_inFlightFence.get());
}
} // namespace nc::graphics