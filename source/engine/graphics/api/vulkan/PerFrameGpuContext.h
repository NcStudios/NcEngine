#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Device;

/** This class will hold all the GPU synchronizaton and Vulkan interface data required for a single frame. (Semaphores, command buffers, etc.). 
 * The semaphores deal solely with the GPU. Since rendering to an image taken from the swapchain and returning that image back to the swap chain are both asynchronous, the semaphores below tell the GPU when either step can begin for a single image.
 * The fence synchronizes GPU - CPU and limits Vulkan to submitting only N frame-render jobs to the command queues. (where N is count of PerFrameGpuContext objects parent owns)
 * The fence m_inFlightFence prevents more frame-render jobs than fences from being submitted until one frame-render job completes. */
class PerFrameGpuContext
{
    public:
        PerFrameGpuContext(const Device& device, uint32_t index);

        void WaitForSync(); // Waits until the fence has signaled
        void ResetSync() noexcept; // Resets the fence
        void SubmitBufferToQueue(vk::Queue graphicsQueue); // Submits the recorded commands in the command buffer

        vk::Fence Fence() const noexcept { return m_inFlightFence.get(); }
        vk::Semaphore ImageAvailableSemaphore() noexcept { return m_imageAvailableSemaphore.get(); }
        vk::Semaphore RenderFinishedSemaphore() noexcept { return m_renderFinishedSemaphore.get(); }
        vk::CommandBuffer* CommandBuffer() noexcept { return &(m_commandBuffer.get()); }
        auto Index() noexcept { return m_index; }

    private:
        vk::Device m_device;

        /** Synchronization */
        vk::UniqueSemaphore m_imageAvailableSemaphore; /** Controls when the swapchain image can be written to. */
        vk::UniqueSemaphore m_renderFinishedSemaphore; /** Controls when the swapchain image can be presented back to the swapchain. */
        vk::UniqueFence m_inFlightFence; /** Synchronizes the submission of the queues from the CPU with the completion of the queues on the GPU. */

        /** Buffer to record render commands to for the frame */
        vk::UniqueCommandPool m_commandPool;
        vk::UniqueCommandBuffer m_commandBuffer;

        uint32_t m_index;
};
} // namespace nc::graphics