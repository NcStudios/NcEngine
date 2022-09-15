#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
/** This class will hold all the GPU synchronizaton and Vulkan interface data required for a single frame. (Semaphores, Command Buffers, etc.). 
 * The semaphores deal solely with the GPU. Since rendering to an image taken from the swapchain and returning that image back to the swap chain are both asynchronous, the semaphores below tell the GPU when either step can begin for a single image.
 * The fence synchronizes GPU - CPU and limits Vulkan to submitting only N frame-render jobs to the command queues. (where N is count of PerFrameGpuContext objects parent owns)
 * The fence m_inFlightFence prevents more frame-render jobs than fences from being submitted until one frame-render job completes. 
 * @todo: Add the command buffers to this class. Move this class out of the swapchain. **/
class PerFrameGpuContext
{
    public:
        PerFrameGpuContext(vk::Device device);
        ~PerFrameGpuContext() noexcept;

        void Wait(); // Waits until the fence has signaled
        void Reset(); // Resets the fence
        vk::Fence Fence() { return m_inFlightFence; }
        vk::Semaphore& ImageAvailableSemaphore() { return m_imageAvailableSemaphore; }
        vk::Semaphore& RenderFinishedSemaphore() { return m_renderFinishedSemaphore; }

    private:
        vk::Device m_device;
        vk::Semaphore m_imageAvailableSemaphore; // Controls when the swapchain image can be written to.
        vk::Semaphore m_renderFinishedSemaphore; // Controls when the swapchain image can be presented back to the swapchain.
        vk::Fence m_inFlightFence; // Synchronizes the submission of the queues from the CPU with the completion of the queues on the GPU.
};
} // namespace nc::graphics