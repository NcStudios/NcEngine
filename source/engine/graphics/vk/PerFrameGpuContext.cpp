#include "PerFrameGpuContext.h"
#include "debug/Utils.h"

namespace nc::graphics
{
PerFrameGpuContext::PerFrameGpuContext(vk::Device device)
    : m_device{device},
      m_imageAvailableSemaphore{m_device.createSemaphore(vk::SemaphoreCreateInfo{})},
      m_renderFinishedSemaphore{m_device.createSemaphore(vk::SemaphoreCreateInfo{})},
      m_inFlightFence{[device]() { return device.createFence(vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled}); }()}
{
}

PerFrameGpuContext::~PerFrameGpuContext() noexcept
{
    m_device.destroySemaphore(m_imageAvailableSemaphore);
    m_device.destroySemaphore(m_renderFinishedSemaphore);
    m_device.destroyFence(m_inFlightFence);
}

void PerFrameGpuContext::Wait()
{
    if (m_device.waitForFences(m_inFlightFence, true, UINT64_MAX) != vk::Result::eSuccess)
    {
        throw NcError("Could not wait for fences to complete.");
    }
}

void PerFrameGpuContext::Reset()
{
    m_device.resetFences(m_inFlightFence);
}
} // namespace nc::graphics