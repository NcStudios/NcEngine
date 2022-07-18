#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <optional>

namespace nc::graphics
{
enum class QueueFamilyType
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
} // namespace nc::graphics
