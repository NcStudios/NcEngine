#include "QueueFamily.h"
#include "debug/NcError.h"

namespace nc::graphics
{
QueueFamilyIndices::QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
    uint32_t queueFamilyCount = 0;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        vk::Bool32 presentSupport = false;
        if (device.getSurfaceSupportKHR(i, surface, &presentSupport) != vk::Result::eSuccess)
        {
            throw NcError("Could not get surface support KHR");
        }

        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            m_graphicsFamily = i;
        }

        if (presentSupport)
        {
            m_presentFamily = i;
        }

        if (IsComplete())
        {
            break;
        }

        ++i;
    }

    if (!m_graphicsFamily.has_value())
    {
        throw NcError("No graphics queue family found on device.");
    }

    if (!m_presentFamily.has_value())
    {
        throw NcError("No present queue family found on device.");
    }

    m_isSeparatePresentQueue = m_presentFamily != m_graphicsFamily ? true : false;
}

bool QueueFamilyIndices::IsComplete() const
{
    return m_graphicsFamily.has_value() && m_presentFamily.has_value();
}

bool QueueFamilyIndices::IsSeparatePresentQueue() const
{
    return m_isSeparatePresentQueue;
}

uint32_t QueueFamilyIndices::GetQueueFamilyIndex(QueueFamilyType type) const
{
    if (!IsComplete())
    {
        throw NcError("QueueFamilyIndices::GetQueueFamilyIndex() - QueueFamilies incomplete.");
    }

    switch (type)
    {
        case QueueFamilyType::GraphicsFamily:
            return m_graphicsFamily.value();
        case QueueFamilyType::PresentFamily:
            return m_presentFamily.value();
    }
    throw NcError("QueueFamilyIndices::GetQueueFamilyIndex() - Chosen queue not present.");
}
} // namespace nc::graphics
