#pragma once

#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vk_mem_alloc.hpp"
#include "imgui/imgui_impl_vulkan.h"

namespace nc::graphics
{
    constexpr uint32_t MaxFramesInFlight = 2u;

    class Device
    {
        public:
            Device(HWND hwnd, HINSTANCE hinstance);

            auto GetDevice()         const -> const vk::Device&         { return m_logicalDevice;  }
            auto GetPhysicalDevice() const -> const vk::PhysicalDevice& { return m_physicalDevice; }
            auto GetSurface()        const -> const vk::SurfaceKHR&     { return m_surface;        }

        private:
            vk::Instance m_instance;
            vk::SurfaceKHR m_surface;
            vk::PhysicalDevice m_physicalDevice;
            vk::Device m_logicalDevice;

            void CreatePhysicalDevice();
    };
}