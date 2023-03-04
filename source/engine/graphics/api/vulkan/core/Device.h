#pragma once

#include "graphics/api/vulkan/QueueFamily.h"

#include <functional>
#include <memory>
#include <span>
#include <vulkan/vulkan.hpp>

namespace nc::graphics
{
class Instance;

class Device
{
    public:
        static auto Create(const Instance& instance, vk::SurfaceKHR surface,
                           std::span<const char* const> requiredExtensions) -> std::unique_ptr<Device>;

        Device(vk::PhysicalDevice physicalDevice,
               const QueueFamilyIndices& queueIndices,
               std::span<const char* const> enabledExtensions);

        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        auto VkDevice() const noexcept -> vk::Device
        {
            return m_device.get();
        }

        auto VkPhysicalDevice() const noexcept -> vk::PhysicalDevice
        {
            return m_physicalDevice;
        }

        auto VkGraphicsQueue() const noexcept -> vk::Queue
        {
            return m_graphicsQueue;
        }

        auto VkPresentQueue() const noexcept -> vk::Queue
        {
            return m_presentQueue;
        }

        void ExecuteCommand(std::function<void(vk::CommandBuffer)>&& function) const;

    private:
        vk::PhysicalDevice m_physicalDevice;
        vk::UniqueDevice m_device;

        QueueFamilyIndices m_queueIndices;
        vk::Queue m_graphicsQueue;
        vk::Queue m_presentQueue;
};
} // namespace nc::graphics