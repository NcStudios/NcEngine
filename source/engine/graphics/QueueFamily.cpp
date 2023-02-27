#include "QueueFamily.h"
#include "ncutility/NcError.h"

namespace nc::graphics
{
QueueFamilyIndices::QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
    auto queueFamilyCount = uint32_t{0};
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    auto queueFamilies = std::vector<vk::QueueFamilyProperties>(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    auto i = uint32_t{0};
    for (const auto& queueFamily : queueFamilies)
    {
        auto presentSupport = vk::Bool32{false};
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
}
} // namespace nc::graphics
