#pragma once

#include <optional>

namespace vk
{
    class PhysicalDevice;
    class SurfaceKHR;
}

namespace nc::graphics
{
    enum class QueueFamilyType : uint8_t
    {
        GraphicsFamily,
        PresentFamily
    };

    class QueueFamilyIndices
    {
        public:
            QueueFamilyIndices(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
            bool IsComplete() const;
            bool IsSeparatePresentQueue() const;
            uint32_t GetQueueFamilyIndex(QueueFamilyType type) const;

        private:
            std::optional<uint32_t> m_graphicsFamily;
            std::optional<uint32_t> m_presentFamily;
            bool m_isSeparatePresentQueue;
    };
}