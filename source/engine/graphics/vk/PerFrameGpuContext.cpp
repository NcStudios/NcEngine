#include "PerFrameGpuContext.h"
#include "utility/NcError.h"

namespace nc::graphics
{
PerFrameGpuContext::PerFrameGpuContext(vk::Device device)
    : m_device{device},
      m_imageAvailableSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_renderFinishedSemaphore{m_device.createSemaphoreUnique(vk::SemaphoreCreateInfo{})},
      m_inFlightFence{m_device.createFenceUnique(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled})}
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