#pragma once

#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Instance 
    {
        public:
            Instance(HWND hwnd, HINSTANCE hinstance);
            const vk::SurfaceKHR& GetSurface() const noexcept;
            const vk::Instance& GetInstance() const noexcept;

        private:
            vk::UniqueInstance m_instance;
            vk::UniqueSurfaceKHR m_surface;
    };
}