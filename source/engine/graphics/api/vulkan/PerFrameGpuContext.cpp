#include "PerFrameGpuContext.h"
#include "core/Device.h"

#include "ncutility/NcError.h"

namespace
{
auto CreateCommandPool(const nc::graphics::Device& device) -> vk::UniqueCommandPool
{
    auto poolInfo = vk::CommandPoolCreateInfo
    {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        device.GetQueueIndices().GraphicsFamilyIndex()
    };

    return device.VkDevice().createCommandPoolUnique(poolInfo);
}

auto CreateCommandBuffer(const nc::graphics::Device& device, vk::CommandPool commandPool) -> vk::UniqueCommandBuffer
{
    auto allocInfo = vk::CommandBufferAllocateInfo
    {
        commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    };

    auto vkDevice = device.VkDevice();
    return std::move(vkDevice.allocateCommandBuffersUnique(allocInfo).front());
}
} // anonymous namespace

namespace nc::graphics
{
PerFrameGpuContext::PerFrameGpuContext(const Device& device, uint32_t index)
    : m_device{device.VkDevice()},
      m_imageAvailableSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_renderFinishedSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_inFlightFence{m_device.createFenceUnique(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled})},
      m_commandPool{::CreateCommandPool(device)},
      m_commandBuffer{::CreateCommandBuffer(device, m_commandPool.get())},
      m_index{index}
{
}

void PerFrameGpuContext::WaitForSync()
{
    if (m_device.waitForFences(m_inFlightFence.get(), true, UINT64_MAX) != vk::Result::eSuccess)
    {
        throw NcError("Could not wait for fences to complete.");
    }
}

void PerFrameGpuContext::ResetSync() noexcept
{
    m_device.resetFences(m_inFlightFence.get());
}

void PerFrameGpuContext::SubmitBufferToQueue(vk::Queue graphicsQueue)
{
    vk::Semaphore waitSemaphores[] = {ImageAvailableSemaphore()}; // Which semaphore to wait on before execution begins
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Which stage of the pipeline to wait in
    vk::Semaphore signalSemaphores[] = {RenderFinishedSemaphore()}; // Which semaphore to signal when execution completes

    vk::SubmitInfo submitInfo{};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(CommandBuffer());
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    m_device.resetFences(Fence());
    graphicsQueue.submit(submitInfo, Fence());
}
} // namespace nc::graphics
