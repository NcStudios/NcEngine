#pragma once

#include "platform/win32/NcWin32.h"

#include <string_view>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

namespace nc::graphics
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

        auto CreateSurface(HWND hwnd, HINSTANCE hinstance) const -> vk::UniqueSurfaceKHR;
        auto GetPhysicalDevices() const -> std::vector<vk::PhysicalDevice>;

    private:
        vk::UniqueInstance m_instance;
};
} // nc::graphics
