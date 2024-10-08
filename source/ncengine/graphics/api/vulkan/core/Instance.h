#pragma once

#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace nc::graphics::vulkan
{
class Instance
{
    public:
        Instance(std::string_view appName, uint32_t appVersion,
                 uint32_t apiVersion, bool enableValidationLayers);

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;
        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&) = delete;

        auto VkInstance() const noexcept -> vk::Instance
        {
            return m_instance.get();
        }

        auto CreateSurface(GLFWwindow* window) const -> vk::UniqueSurfaceKHR;
        auto GetPhysicalDevices() const -> std::vector<vk::PhysicalDevice>;

    private:
        vk::UniqueInstance m_instance;
};
} // nc::graphics::vulkan
