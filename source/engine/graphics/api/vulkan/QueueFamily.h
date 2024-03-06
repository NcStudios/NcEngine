#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <optional>
#include <set>

namespace nc::graphics::vulkan
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

        auto IsComplete() const noexcept -> bool
        {
            return m_graphicsFamily.has_value() && m_presentFamily.has_value();
        }

        auto IsSeparatePresentQueue() const noexcept -> bool
        {
            return m_graphicsFamily != m_presentFamily;
        }

        auto GetQueueFamilyIndex(QueueFamilyType type) const -> uint32_t
        {
            return type == QueueFamilyType::GraphicsFamily ? m_graphicsFamily.value() : m_presentFamily.value();
        }

        auto GraphicsFamilyIndex() const -> uint32_t
        {
            return m_graphicsFamily.value();
        }

        auto PresentFamilyIndex() const -> uint32_t
        {
            return m_presentFamily.value();
        }

        auto GetUniqueIndices() const -> std::set<uint32_t>
        {
            return std::set<uint32_t>{m_graphicsFamily.value(), m_presentFamily.value()};
        }

    private:
        std::optional<uint32_t> m_graphicsFamily;
        std::optional<uint32_t> m_presentFamily;
};
} // namespace nc::graphics::vulkan
